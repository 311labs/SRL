#ifndef __SRL_STRINGTEST__
#define __SRL__STRINGTEST__

#include "srl/String.h"
#include "srl/Test.h"

namespace SRL
{
    namespace Test
    {
        /** Test our data types */
        class SystemTest : public UnitTest
        {
        public:
        	SystemTest() : UnitTest("System API"){}
	        virtual ~SystemTest(){}
        	virtual void run_tests()
        	{
        		RUN_TEST("Validate Info API Calls", infoTest());
        		RUN_TEST("Validate Run Kill Calls", runkillTest());
        		RUN_TEST("Process Spawning", runProcs());
        	}
        private:
        	/** test system info api */
        	bool infoTest();
        	/** test run kill functions */
        	bool runkillTest();
        	/** test process spawning */
            bool runProcs();

        };
    }
}

#endif // __SRL__STRINGTEST__

