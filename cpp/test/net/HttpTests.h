#ifndef __SRL__HTTPTEST__
#define __SRL__HTTPTEST__

#include "srl/Test.h"

namespace SRL
{
    namespace Test
    {
        
        /** String Unit Testing */
        class HttpTest : public UnitTest
        {
        public:
        	HttpTest() : UnitTest("HTTP"){}
	        virtual ~HttpTest(){}
        	virtual void run_tests()
        	{
        		RUN_TEST("URI Tests", uriTest());
                // RUN_TEST("Socket Address Test", sockaddressTest());
                // RUN_TEST("Simple TCP Socket Test", tcpSocketTest());
                // RUN_TEST("Simple UDP Socket Test", udpSocketTest());
        	}
        private:
        	/** test basic network address (InetAddress) */
        	bool uriTest();
        	/** test socket address (InetSocketAddress) */
        	bool httpClient();
            /** simple tcp socket test */
            bool httpServer();          
            
        };
    }
}

#endif // __SRL__HTTPTEST__

