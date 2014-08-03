#ifndef __SRLHTTPSERVER__
#define __SRLHTTPSERVER__

#include "srl/net/TcpSocket.h"
#include "srl/util/Dictionary.h"
#include "srl/sys/Thread.h"
#include "srl/DateTime.h"
#include "srl/io/ByteBuffer.h"
#include "srl/log/Logger.h"
#include "srl/net/HttpHeaders.h"

namespace SRL
{
    namespace Net
    {        
        class HttpRequestHandler;
        /**
        * HTTP (HyperText Transfer Protocol) is an extensible protocol on
        * top of a reliable stream transport (e.g. TCP/IP).  The protocol
        * recognizes three parts to a request:
        * 
        * 1. One line identifying the request type and path
        * 2. An optional set of RFC-822-style headers
        * 3. An optional data part
        * 
        * The headers and data are separated by a blank line.
        * 
        * This is the simplest implementation of an http request and is really meant
        * to be a base class
        */
        class SRL_API HttpRequest : public System::Thread
        {
        public:
            /**
            * Creates a new HTTP Request Instance with the proper system, server, and request versions
            */
            HttpRequest(System::ThreadManager* manager, HttpRequestHandler &handler);
            virtual ~HttpRequest ();
            
            /** accepts a new connection but does not parse anything */
            void accept(const Socket::Descriptor& desc);
            
            /** returns the protocol string in teh form of protocol/major.minor */
            const String& protocol() const { return _request_version; }
            /** returns the request method (GET, POST, PUT) */
            const String& method() const { return _method; }
            /** returns the request URI */
            const String& uri() const {return _uri; }
            
            /** returns the content as a string */
            const uint32& contentLength(){ return _content_length; };
            /** returns the content as a string, will parse the request */
            const String& getContent();
            
            /** returns the next line in the request */
            const String& readLine();
            
            /** returns the inetsocket address object */
            const InetSocketAddress& remoteAddress();
            
            /** returns true if the header exists in this request */
            bool hasHeader(const String& header) const; 
            /** returns a dictionay containing the http headers included with the request */
            const String& getHeader(const String& header);
            
            /** returns the connected TCP Socket */
            TcpSocket& connection(){ return *_socket; }
            
            /** 
            * start sending the response header
            * sends the first line of the response: 
            * REQUEST_VERSION STATUS_CODE REASON
            */
            void startHeader(const uint32& code, const String& reason);
            
            /** send a response header */
            void sendHeader(const String& key, const String& value);
            
            /** end the RESPONSE headers by sending the blank lines */
            void endHeader();
            
            /** simple way to send content back assumes no headers have been sent */
            void sendResponse(const String& content, const String& content_type);
            
            /** send an error response back */
            void sendError(const uint32& code, const String& reason);
            
            /** close the connection */
            void close();
        
        protected:
            /** parse headers */
            void _parseRequest();
            void _parseHeaders();
            void _parseVersion();
            /** thread callbacks */
            bool run();
            void final();
        
        protected:
            bool _keep_alive;
            /** string to hold the content if parsed via the getContent() method */
            String _content;
            /** content length if available from the header */
            uint32 _content_length;
            // our trusty socket for this request
            TcpSocket *_socket;
            // sequence of MIME-type headers
            HttpHeaders _headers;
            /** uri for the request */
            String _method, _uri, _request_version;
            /** the request version numbers */
            uint32 _major, _minor;
            /** the request handler that handles incoming calls */
            HttpRequestHandler& _handler;
            /** datetime object */
            DateTime _date;
            /** logger */
            Log::Logger &_log;
            /** string the holds the address and port for logging purposes */
            String _log_prefix;
        };
        
        /**
        * This is the HTTP Request handler base class.  An instance of this class should be
        * given to an HttpServer.  It will then receive callbacks from the HttpServer
        * on the appropriate callback method based on the type of request.
        * 
        * By itself this class does not support any functionality.
        */
        class SRL_API HttpRequestHandler
        {
        public:
            /**
            * Default Constructor
            * 
            * @param protocol_major major version for the http protocol this handler supports
            * @param protocol_minor minor version for the http procotol this handler supports
            */
            HttpRequestHandler(uint32 protocol_major=0, uint32 protocol_minor=9);
            /**
            * Specific Logger Constructor
            * 
            * @param logger specifify a logger to use 
            * @param protocol_major major version for the http protocol this handler supports
            * @param protocol_minor minor version for the http procotol this handler supports
            */
            HttpRequestHandler(Log::Logger& logger, uint32 protocol_major=0, uint32 protocol_minor=9);            
            /** destructor */
            virtual ~HttpRequestHandler (){};
            
            /**
            * HEAD REQUEST
            * Asks for the response identical to the one that would correspond to a 
            * GET request, but without the response body. This is useful for retrieving 
            * meta-information written in response headers, without having to transport
            * the entire content.
            */
            virtual bool http_head(HttpRequest& request){ return false; }
            /**
            * GET REQUEST
            * Requests a representation of the specified resource. By far the most common
            * method used on the Web today. Should not be used for operations that cause
            * side-effects (using it for actions in web applications is a common misuse).
            */
            virtual bool http_get(HttpRequest& request){ return false; }
            /**
            * Submits data to be processed (e.g. from an HTML form) to the identified 
            * resource. The data is included in the body of the request. This may result
            * in the creation of a new resource or the updates of existing resources 
            * or both.
            */
            virtual bool http_post(HttpRequest& request){ return false; }
            /**
            * Uploads a representation of the specified resource.
            */
            virtual bool http_put(HttpRequest& request){ return false; }
            /**
            * Deletes the specified resource.
            */
            virtual bool http_delete(HttpRequest& request){ return false; }
            /**
            * Echoes back the received request, so that a client can see what intermediate
            * servers are adding or changing in the request.
            */
            virtual bool http_trace(HttpRequest& request){ return false; }
            /**
            * Returns the HTTP methods that the server supports. This can be used to check
            * the functionality of a web server.
            */
            virtual bool http_options(HttpRequest& request){ return false; }
            /**
            * UNKNOWN REQUEST
            * The request type is not one of the known request types 
            */
            virtual bool http_unknown(HttpRequest& request){ return false; }
            
            /** returns the http servers version string ie:'SRL::HttpServer 3.0' */
            const String& serverVersion() const { return _server_version; }
            /** returns the supported request version ie: 'HTTP/1.0' */
            const String& protocolVersion() const { return _protocol_version; }
            /** returns the protocol version major number */
            const uint32& protocolMajor() const { return _major; }
            /** returns the protocol version major number */
            const uint32& protocolMinor() const { return _minor; }
            /** returns the logger for this handler */
            Log::Logger& getLogger(){ return _log; }
        protected:
            /** protocol version numbers */
            uint32 _major, _minor;
            /** the current HTTP Server version */
            String _server_version;
            /** specify the supported request version HTTP/1.0 or HTTP/1.1 */
            String _protocol_version;
            /** logger */
            Log::Logger &_log;
        };        
        
        
        /**
        * Simple Http Request Handler that implements 0.9 Protocol
        */
        class SRL_API SimpleHttpRequestHandler : public HttpRequestHandler
        {
        public:
            SimpleHttpRequestHandler(const String& path);
            virtual ~SimpleHttpRequestHandler(){};

            /**
            * GET REQUEST
            * Requests a representation of the specified resource. By far the most common
            * method used on the Web today. Should not be used for operations that cause
            * side-effects (using it for actions in web applications is a common misuse).
            */
            virtual bool http_get(HttpRequest& request);

        private:
            /** path to serve from */
            String _path;
        };
        
        /**
        * Base Class to be used by http servers
        * 
        */
        class SRL_API HttpServer : protected System::Thread, protected System::ThreadManager
        {
        public:
            HttpServer(HttpRequestHandler& handler, uint32 max_cons=10);
            virtual ~HttpServer ();

            /** 
            * start the http server on 0.0.0.0 port 80 
            * If know handler is set we use the default handler and set the path to
            * the current working directory
            */
            void start();
            /** start the http server on the host and port specified */
            void start(const String& host, const uint32& port);
            
            /** stop the http server */
            void stop();
            
            /** is the server still running */
            bool isRunning() const { return isAlive(); }
            
            /** Starts the Server on the calling thread so this function should block forvever */
            void serveForever();
            void serveForever(const String& host, const uint32& port);
        
        protected:
            /** called by a thread when it has finished its task */
            virtual void task_completed(System::Thread* thread);
            
            /** attempts to get an available request thread */
            HttpRequest* _getRequest();
            /** creates a new request thread when we run out */
            HttpRequest* _createRequest();
            
            /** event listener thread functions */
    		bool initial();
    		bool run();
    		void final();
            
        protected:
            /** InetSocketAddress with the host and port to serve from */
            InetSocketAddress _socket_address;
            /** the request handler that handles incoming calls */
            HttpRequestHandler& _handler;
            /** socket to listen for inccoming requests on */
            Socket _socket;
            /** logger */
            Log::Logger &_log;
            
            System::Mutex _pool_lock;
            System::Semaphore _blocker;
            uint32 _size, _max_size; 
            Util::Vector<HttpRequest*> _available_pool;
            Util::Vector<HttpRequest*> _busy_pool;

        };
        
        
    } /* Net */
    
} /* SRL */


#endif