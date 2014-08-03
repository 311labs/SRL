#ifndef _SRL_XML_RPCPROTO_
#define _SRL_XML_RPCPROTO_

#include "srl/rpc/RpcProtocol.h"
#include "srl/net/HttpServer.h"

using namespace SRL::Net;

namespace SRL
{
    namespace XML
    {
        /**
        * XmlRpcProtocol
        * Implementation of the XML-RPC Protocol
        */
        class SRL_API XmlRpcProtocol : public Net::HttpRequestHandler, public RPC::RpcProtocol
        {
        public:
            /** default contstructor */
            XmlRpcProtocol();
            /** default destructor */
            virtual ~XmlRpcProtocol();

            /**
            * XML-RPC Only supports the post HTTP Method
            * 
            * @overrides Net::HttpRequestHandler::http_post
            */
            bool http_post(HttpRequest& request);
            /**
            * Sends an XML-RPC Fault message
            */
            bool sendError(Net::HttpRequest& request, const String &e);
            bool sendError(Net::HttpRequest& request, SRL::Errors::Exception &e);
            /**
            * Parses out an RpcValue if the parser.currentTag == value
            */
            RPC::RpcValue* parseValue(XML::Parser& parser);
        protected:
            /** logger */
            Log::Logger &_log;            
            
        };
        
    } /* XML */
    
} /* SRL */


#endif /* _SRL_XML_RPCPROTO_ */
