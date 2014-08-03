#ifndef __SRL_XMLTEST__
#define __SRL_XMLTEST__

#include "srl/String.h"
#include "srl/Test.h"

namespace SRL
{
    namespace Test
    {
        /** XML Unit Testing */
        class XmlTest : public UnitTest
        {
        public:
            XmlTest() : UnitTest("XML"){}
            virtual ~XmlTest(){}
            virtual void run_tests()
            {
                RUN_TEST("Parser", parserTest());
                // RUN_TEST("XML-RPC", xmlRPC());
                // RUN_TEST("Encoding", encoding());
                // RUN_TEST("Validation", validation());
            }
        private:
            /** test string constructors */
            bool parserTest();


        };
    }
}

#endif // __SRL_XMLTEST__

