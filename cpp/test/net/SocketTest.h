#ifndef __SRL_SOCKETTEST__
#define __SRL__SOCKETTEST__

#include "srl/Test.h"

namespace SRL
{
    namespace Test
    {
        
        /** String Unit Testing */
        class SocketTest : public UnitTest
        {
        public:
        	SocketTest() : UnitTest("Socket"){}
	        virtual ~SocketTest(){}
        	virtual void run_tests()
        	{
        		RUN_TEST("Network Address Test", netaddressTest());
        		RUN_TEST("Socket Address Test", sockaddressTest());
        		RUN_TEST("Simple TCP Socket Test", tcpSocketTest());
        		RUN_TEST("Simple UDP Socket Test", udpSocketTest());
        	}
        private:
        	/** test basic network address (InetAddress) */
        	bool netaddressTest();
        	/** test socket address (InetSocketAddress) */
        	bool sockaddressTest();
            /** simple tcp socket test */
            bool tcpSocketTest();
            /** simple udp socket test */
            bool udpSocketTest();            
            
        };
    }
}

#endif // __SRL__STRINGTEST__

