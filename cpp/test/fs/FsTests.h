#ifndef __SRL_FSTEST__
#define __SRL_FSTEST__

#include "srl/String.h"
#include "srl/Test.h"

namespace SRL
{
    namespace Test
    {
        /** Test our data types */
        class FsTest : public UnitTest
        {
        public:
        	FsTest() : UnitTest("File System Tests"){}
	        virtual ~FsTest(){}
        	virtual void run_tests()
        	{
        		RUN_TEST("FS API", apiTest());
        		RUN_TEST("Directories", dirTest());
        		RUN_TEST("Text Files", txtfileTest());
        		RUN_TEST("FS Object", fsobjectTest());
        		RUN_TEST("Binary Files", binfileTest());
        		RUN_TEST("FS Object Monitor", monitorTest());
        	}
        private:
        	/** test to make sure our data types are the correct sizes */
        	bool apiTest();
        	/** test Date Class */
        	bool dirTest();
        	/** test Time Class */
        	bool txtfileTest();
        	/** test the Timer Class */
        	bool fsobjectTest();
        	/** test DateTime Class */
        	bool binfileTest();
        	/** test DateTime Class */
        	bool monitorTest();
        };
    }
}

#endif // __SRL_FSTEST__

