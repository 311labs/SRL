#ifndef _DICTTEST_H_
#define _DICTTEST_H_

#include "srl/String.h"
#include "srl/Test.h"

namespace SRL
{
    namespace Test
    {
        /** Test our data types */
        class DictTest : public UnitTest
        {
        public:
        	DictTest() : UnitTest("Dictionary"){}
	        virtual ~DictTest(){}
        	virtual void run_tests()
        	{
        		RUN_TEST("Create Add Remove Get", basicTest());
        		//RUN_TEST("Iterators", iterationTest());
        		//RUN_TEST("Sorting", sortTest());
        		//RUN_TEST("List Interface", listTest());
        	}
        private:
        	/** Create Add Remove Get */
        	bool basicTest();
        	/** Iterators */
        	bool iterationTest();
        	/** Sorting */
        	bool sortTest();
        	/** List Interface */
        	bool listTest();
        };
    }
}


#endif /* _DICTTEST_H_ */


