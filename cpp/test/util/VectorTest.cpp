
#include "VectorTest.h"

#include "srl/util/Vector.h"

using namespace SRL;
using namespace SRL::Test;

bool VectorTest::basicTest()
{
    Util::Vector<int> int_list;
    int_list.add(5); // 0
    int_list.add(4); // 1
    int_list.add(3); // 2
    int_list.add(2); // 3
    int_list.add(1); // 4
    int_list.add(0); // 5
    
    TEST_ASSERT(int_list.size() == 6)
    TEST_ASSERT(int_list[0] == 5)
    TEST_ASSERT(int_list[3] == 2)
    TEST_ASSERT(int_list[1] == 4)
    int_list.remove(4);
    TEST_ASSERT(int_list[1] == 3)
    int_list.removeAt(2);
    TEST_ASSERT(int_list[2] == 1)
    int_list.set(2, 15);
    TEST_ASSERT(int_list[2] == 15)
    TEST_ASSERT(int_list.positionOf(15) == 2)
    TEST_ASSERT(int_list.size() == 4)
    int_list.addAt(1, 22);
    //Console::write("\nlist dump: [");
    //for (int i=0;i<int_list.size(); ++i)
    //    Console::format(" %d:%d", i, int_list[i]);
    TEST_ASSERT(int_list.size() == 5)
    TEST_ASSERT(int_list[1] == 22)
    TEST_ASSERT(int_list[2] == 3)
    int_list.clear();
    TEST_ASSERT(int_list.size() == 0)

    // now lets create 1000 items
    for (int i=0;i<1000;++i)
        int_list.add(i);
        
    // Console::write("\nlist dump: [");
    // for (int i=0;i<int_list.size(); ++i)
    //    Console::format(" %d:%d", i, int_list[i]);        
    TEST_ASSERT(int_list.size() == 1000)
    TEST_ASSERT(int_list[800] == 800)
    int_list.remove(20);
    TEST_ASSERT(int_list[20] == 21)
    TEST_ASSERT(int_list[800] == 801)
    uint32 total = int_list.size();
    for (uint32 i=0;i<total;++i)
    {
        int_list.removeAt(0);
    }

    TEST_ASSERT(int_list.size()==0)

    // now lets do pointers
    String name1 = "Bob Jones";
    String name2 = "Marco Gomez";
    String name3 = "Richard Lump";
    String name4 = "Isabella Desilva";
    Util::Vector<String*> str_list;

    str_list.add(&name1);
    str_list.add(&name2);
    str_list.add(&name3);
    str_list.add(&name4);
    TEST_ASSERT(str_list.size() == 4)
    TEST_ASSERT((*str_list[1]) == "Marco Gomez")
    (*str_list[1]) = "Mario Anders";
    TEST_ASSERT((*str_list[1]) == "Mario Anders")
    TEST_ASSERT(name2 == "Mario Anders")
    TEST_ASSERT(str_list.containsValue(&name1))
    str_list.remove(&name2);
    TEST_ASSERT((*str_list[1]) == "Richard Lump")

    // now lets test with non Pointer Objects
    Util::Vector<String> str_list2;
    str_list2.add("Bob Jones");
    str_list2.add("Marco Gomez");
    str_list2.add("Richard Lump");
    str_list2.add("Isabella Desilva");
    TEST_ASSERT(str_list2.size() == 4) 
    TEST_THROWS(str_list2.get(4), Errors::IndexOutOfBoundsException)
    TEST_ASSERT(str_list2.containsValue("Bob Jones"))

    
    return true;
}

bool VectorTest::iterationTest()
{
    TEST_ASSERT(false)
    return true;
}

bool VectorTest::sortTest()
{
    TEST_ASSERT(false)
    return true;
}

bool VectorTest::listTest()
{
    TEST_ASSERT(false)
    return true;
}


