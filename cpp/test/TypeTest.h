#ifndef __SRL_TYPETEST__
#define __SRL_TYPETEST__

#include "srl/String.h"
#include "srl/Test.h"

namespace SRL
{
    namespace Test
    {
        /** Test our data types */
        class TypeTest : public UnitTest
        {
        public:
        	TypeTest() : UnitTest("Basic Types"){}
	        virtual ~TypeTest(){}
        	virtual void run_tests()
        	{
        		RUN_TEST("Validate Type Sizes", sizeTest());
        		RUN_TEST("Date Logic", dateTest());
        		RUN_TEST("Time Logic", timeTest());
        		RUN_TEST("Timer Logic", timerTest());
        		RUN_TEST("DateTime Logic", dtTest());
        	}
        private:
        	/** test to make sure our data types are the correct sizes */
        	bool sizeTest();
        	/** test Date Class */
        	bool dateTest();
        	/** test Time Class */
        	bool timeTest();
        	/** test the Timer Class */
        	bool timerTest();
        	/** test DateTime Class */
        	bool dtTest();
        };
    }
}

#endif // __SRL_TYPETEST__

