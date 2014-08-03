#ifndef __SRL_JSONTEST__
#define __SRL_JSONTEST__

#include "srl/String.h"
#include "srl/Test.h"

namespace SRL
{
    namespace Test
    {
        /** XML Unit Testing */
        class JsonTest : public UnitTest
        {
        public:
            JsonTest() : UnitTest("JSON"){}
            virtual ~JsonTest(){}
            virtual void run_tests()
            {
                RUN_TEST("Parser", parserTest());
            }
        private:
            /** test string constructors */
            bool parserTest();


        };
    }
}

#endif // __SRL_JSONTEST__

