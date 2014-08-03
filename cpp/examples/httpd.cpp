#ifndef _SRL_HTTPD_
#define _SRL_HTTPD_

#include "srl/app/ArgumentParser.h"
#include "srl/net/HttpServer.h"
#include "srl/Console.h"
#include "srl/log/Logger.h"
#include "srl/xml/XmlRpcProtocol.h"
#include "srl/rpc/RpcSystemService.h"

using namespace SRL;
using namespace SRL::Net;
using namespace SRL::RPC;

void runXmlRpcServer(const String& host, uint32 port)
{
    XML::XmlRpcProtocol handler;
    // RpcDateTimeMethod rpc_dt_method;
    //     RpcService system_service("system");
    //     system_service.addMethod(rpc_dt_method);
    //     handler.addService(system_service);
    RpcSystemService system_service;
    handler.addService(system_service);
    
    HttpServer server(handler);

    try
    {
        server.serveForever(host, port);
    }
    catch (SRL::Errors::Exception &e)
    {
        Console::writeLine(e.message());
    }
}

void runSimpleHTTP(const String& host, uint32 port)
{
    SimpleHttpRequestHandler handler("/Users/ians/");
    HttpServer server(handler);
    
    try
    {
        server.serveForever(host, port);
    }
    catch (SRL::Errors::Exception &e)
    {
        Console::writeLine(e.message());
    }    
}

int main (int argc, char const* argv[])
{
    int major, minor, revision;
    SRL::GetVersion(major, minor, revision);
    String srl_version = String::Format("%d.%d.%d", major, minor, revision);
    
    App::ArgumentParser argparser("SRL HTTP Server", "DESCRIPTION", srl_version);
    argparser.add("verbose", "-v, --verbose", "display verbose output");
    argparser.add("port", "-p, --port", "port to run on", "80");
    argparser.add("host", "-h, --host", "interface to run on", "0.0.0.0");
    argparser.add("xmlrpc", "-x, --xmlrpc", "run the xml-rpc test server");
    argparser.add("help", "--help", "display this output");
    argparser.parse(argc, argv);
     
    if (argparser.get("verbose")->wasParsed())
    {
        Log::Logger::SetGlobalPriorityLevel(Log::Logger::ALL);
    }

    uint32 port = static_cast<uint32>(argparser.get("port")->getIntValue());
    if (argparser.get("xmlrpc")->wasParsed())
    {
        runXmlRpcServer(argparser.get("host")->getValue(), port);
    }
    else
    {
        runSimpleHTTP(argparser.get("host")->getValue(), port);
    }
    
    return 0;
}


#endif /* _SRL_HTTPD_ */
