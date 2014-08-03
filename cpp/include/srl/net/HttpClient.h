#ifndef _SRL_HTTPCLIENT_
#define _SRL_HTTPCLIENT_

#include "srl/String.h"
#include "srl/net/TcpSocket.h"
#include "srl/net/URI.h"
#include "srl/net/HttpHeaders.h"

namespace SRL
{
    namespace Net
    {
        /**
        * HttpClient
        * Logic to make standard HTTP requests to an HTTP server
        * This class is not Thread Safe
        */
        class SRL_API HttpClient
        {
        public:
            /** default contstructor */
            HttpClient();
            /** default destructor */
            virtual ~HttpClient();
            
            /**
            * Will attempt to perform an HTTP GET on the specified host returning
            * the contents into the output string
            * 
            * @param host   remote hosts name or ip with option port in the format of "address" or "address:port"
            * @param uri    resource path to retrieve
            * @param output string where the output will be stored and returned
            * @return String    returns a reference to the output string
            */
            String& get(const URI& uri, String& output);

            /**
            * Will attempt to perform an HTTP GET on the specified host saving
            * the result to the specified location
            * 
            * @param host   remote hosts name or ip with option port in the format of "address" or "address:port"
            * @param uri    resource path to retrieve
            * @param output string where the output will be stored and returned
            * @return String    returns a reference to the output string
            */
            void saveGet(const URI& uri);
            
            // TODO add saveAllGet
            
        protected: // internal member functions
            /** parse the request headers */
            
            
        protected: // internal members
            /** the socket to use for the operation */
            TcpSocket* _socket;
            // map of MIME-type headers
            HttpHeaders _headers;
            /** logger */
            Log::Logger &_log;            
        };
        
        
        
    } /* Net */
    
} /* SRL */



#endif /* _SRL_HTTPCLIENT_ */
