#ifndef __SRL_VECTORTEST__
#define __SRL_VECTORTEST__

#include "srl/String.h"
#include "srl/Test.h"

namespace SRL
{
    namespace Test
    {
        /** Test our data types */
        class VectorTest : public UnitTest
        {
        public:
        	VectorTest() : UnitTest("Vector"){}
	        virtual ~VectorTest(){}
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

#endif // __SRL_VECTORTEST__

