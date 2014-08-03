#ifndef _SRL_HTTPHEADERS_
#define _SRL_HTTPHEADERS_

#include "srl/util/Dictionary.h"
#include "srl/net/TcpSocket.h"
#include "srl/log/Logger.h"

namespace SRL
{
    namespace Net
    {
        typedef Util::Dictionary<String, String> HttpHeaderMap;
        /**
        * HttpHeaders
        * Handles the parsing of HTTP headers
        * 
        */
        class SRL_API HttpHeaders : public HttpHeaderMap
        {
        public:
            /** default contstructor */
            HttpHeaders(Log::Logger& logger);
            /** default destructor */
            virtual ~HttpHeaders();
            
            /** parse the http headers using the passed in TCP Socket, clears existing headers */
            void parse(TcpSocket &tcp_socket);
            
            /** return the value as an int */
            int getAsInt(const String& key){ return String::Int(get(key)); }
            /** return as double */
            double getAsDouble(const String& key){ return String::Double(get(key)); }
            
            
        protected:
            Log::Logger& _log;
            String _buffer;
        };
        
        
        
    } /* Net */
    
} /* SRL */



#endif /* _SRL_HTTPHEADERS_ */
