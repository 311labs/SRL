#ifndef __SRL_STRINGTEST__
#define __SRL__STRINGTEST__

#include "srl/String.h"
#include "srl/Test.h"

namespace SRL
{
    namespace Test
    {
        /** String Unit Testing */
        class StringTest : public UnitTest
        {
        public:
            StringTest() : UnitTest("String"){}
            virtual ~StringTest(){}
            virtual void run_tests()
            {
                RUN_TEST("Constructors", construct());
                RUN_TEST("Assignments", assignment());
                RUN_TEST("Access", access());
                RUN_TEST("Conversion", conversion());
                RUN_TEST("Searn and Replace", search());
                RUN_TEST("STL String Support", std_interface());
                RUN_TEST("Comparison", comparison());
            }
        private:
            /** test string constructors */
            bool construct();
            /** test string assigment operations */
            bool assignment();
            /** test string access interface */
            bool access();
            /** test string conversion logic */
            bool conversion();
            /** test string searching */
            bool search();
            /** test STL std::string compliance */
            bool std_interface();
            /** test comparison operations */
            bool comparison();

        };
    }
}

#endif // __SRL__STRINGTEST__

