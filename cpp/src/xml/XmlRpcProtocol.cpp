#include "srl/xml/XmlRpcProtocol.h"

using namespace SRL;
using namespace SRL::XML;
using namespace SRL::RPC;
using namespace SRL::Net;

const char* METHOD_RESPONSE_HEADER = "<?xml version=\"1.0\"?>\n<methodResponse><params>";
const char* METHOD_RESPONSE_FOOTER = "</params></methodResponse>\n";
const char* METHOD_RESPONSE = "<?xml version=\"1.0\"?>\n<methodResponse><params>%s</params></methodResponse>\n";
const char* METHOD_FAULT = "<?xml version=\"1.0\"?>\n<methodResponse><fault>%s</fault></methodResponse>\n";

XmlRpcProtocol::XmlRpcProtocol() : 
HttpRequestHandler(1, 0),
RpcProtocol("XML-RPC"),
_log(Log::Logger::GetInstance("XML-RPC"))
{
    _log.info("XML-RPC Protocol Started");
}

XmlRpcProtocol::~XmlRpcProtocol()
{
    
}

bool XmlRpcProtocol::sendError(HttpRequest& request, const String &e)
{
    _log.error(e);
    RpcStructValue fault;
    fault.set("faultCode", -1);
    fault.set("faultString", String::Format("%s: %s", System::GetHostName().text(), e.text()));
    request.sendResponse(String::Format(METHOD_FAULT, fault.asXml().text()), "text/xml");
    return true;
}

bool XmlRpcProtocol::sendError(HttpRequest& request, SRL::Errors::Exception &e)
{
    _log.error(e);
    RpcStructValue fault;
    fault.set("faultCode", -1);
    fault.set("faultString", e.message());
    request.sendResponse(String::Format(METHOD_FAULT, fault.asXml().text()), "text/xml");
    return true;
}

RpcValue* XmlRpcProtocol::parseValue(XML::Parser& parser)
{
    // TODO move FromXML logic into this function
    return RpcValue::FromXml(parser);
}

bool XmlRpcProtocol::http_post(HttpRequest& request)
{
    _log.info("xml-rpc post: %s", request.uri().text());
    if (request.uri() != "/RPC2")
        return sendError(request, "invalid XML-RPC method call syntax: invalid URI");

    if (!request.hasHeader("content-type"))
        return sendError(request, "invalid XML-RPC method call syntax: no content-type");
    
    if (request.contentLength() <= 0)
        return sendError(request, "invalid XML-RPC method call syntax: no content");
    
    if (request.getHeader("content-type") == "text/html")
        return sendError(request, "invalid XML-RPC method call syntax: bad content-type");

    try
    {
        XML::Parser parser(request.getContent());
        XML::Parser::Element* element = parser.getNextElement();
        
        bool is_methodCall = false;
        for(uint32 i=0; i<4; ++i)
        {
            if (element->name() == "methodCall")
            {
                is_methodCall = true;
                break;
            }
            element = parser.getNextElement();
        }
        
        if (!is_methodCall || (parser.getNextElement()==NULL) || (element->name() != "methodName"))
            throw SRL::Errors::IOException("failed to parse method name");
        
        // now the current tag should be the method name
        if (!element->hasContent())
            throw SRL::Errors::IOException("empty XML-RPC method name");
        
        // now lets break up the method name into two peices (service, method)
        if (element->getContent().find('.') == -1)
            throw SRL::Errors::IOException("no service name specified");
        
        String service_name = element->getContent().section('.', 0);
        String method_name = element->getContent().section('.', 1);
        _log.info("service: '%s'   method: '%s'", service_name.text(), method_name.text());
        
        // check if service name is system 
        if (service_name == "system")
        {
            // now check if the method name is one of our XML-RPC standard methods
            if (method_name == "listMethods")
            {
                ScopedPointer<RpcArrayValue> list(listMethods());
                request.sendResponse(String::Format(METHOD_RESPONSE, list->asXml().text()), "text/html");
                return true;
            }
        }
        
        // now lets check if we have a service to handle this request
        if (!_services.hasKey(service_name))
            throw SRL::Errors::IOException(String::Format("no service with name: %s", service_name.text()));
        
        RpcService* service = _services.get(service_name);
        if (!service->hasMethod(method_name))
            throw SRL::Errors::IOException(String::Format("service:%s does not have method: %s",
                    service_name.text(), method_name.text()));
                    
        // methodname end tag
        parser.getNextElement();
        // this should be the params element
        element = parser.getNextElement();
        if (element->name() != "params")
            throw SRL::Errors::IOException("invalid XML-RPC method call syntax: no params tag found");
        
        RpcArrayValue args;
        while (parser.getNextElement() && (element->name() == "param") && parser.getNextElement() && (element->name() == "value"))
        {                        
            args.add(parseValue(parser));
            // skip param end tag
            parser.getNextElement();
        }
        //_log.debug("parsed %d method parameters", args.size());
        ScopedPointer<RpcValue> result(service->execute(method_name, &args));
        String xml_response =  METHOD_RESPONSE_HEADER;
        if (!result.isNULL())
            xml_response += result->asXml();
        else
            xml_response += "<value><nil/></value>";
        xml_response += METHOD_RESPONSE_FOOTER;
        //printf("%s\n", xml_response.text());
		//_log.info("got here");
		//_log.info(xml_response.text());
        request.sendResponse(xml_response, "text/html");
		System::Sleep(1000);
        return true;
    }
    catch (SRL::Errors::Exception &e)
    {
        return sendError(request, e);
    }
    catch (...)
    {
        return sendError(request, "unknown error in xml request!");
    }
}