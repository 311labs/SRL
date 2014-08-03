
#include "DictTest.h"

#include "srl/util/Dictionary.h"
#include "srl/sys/System.h"
#include "srl/fs/TextFile.h"
#include "srl/math/Random.h"

using namespace SRL;
using namespace SRL::Test;

// int get_index(Util::Dictionary<int, String>::Entry *entry, Util::Dictionary<int, String> &dict)
// {
//     Util::Dictionary<int, String>::Iterator iter = dict.begin();
//     int index = 0;
//     while (iter != dict.end())
//     {
//         if (iter._entry == entry)
//             return index;
//         ++iter;
//         ++index;
//     }
//     return -1;    
// }
// 
// 
// void print_dict(Util::Dictionary<int, String> &dict, const String& output)
// {
//     static int counter = 0;
//     ++counter;
//     String filename = String::Format("%d_tree.png", counter);
//     Util::Dictionary<int, String>::Iterator iter = dict.begin();    
//     Util::Dictionary<int, String>::Entry *entry = dict._root;
//     
//     Console::write("\nnew tree dump: ");
//     Console::writeLine(filename.text());
//     FS::TextFile *txtfile = FS::CreateTextFile("graph.dot", true);
//     
//     txtfile->writeLine("digraph g {");
//     txtfile->writeLine("node [fontcolor=white, style=filled, fontname = \"Arial\", shape = record,height=.1];");
// 
//     String color = "RED";
//     int index = 0;
//     while (iter != dict.end())
//     {
//         entry = iter._entry;
//         if (entry->_color == Util::Dictionary<int, String>::Entry::RED)
//             color = "red";
//         else
//             color = "black";
//             
//         txtfile->formatLine("node%d[fillcolor=\"%s\", label = \"<f0> |<f1> %d|<f2> \"]", index, color.text(), entry->hash());
//         ++iter;
//         ++index;
//     }
// 
//     iter = dict.begin();
//     index = 0;
//     while (iter != dict.end())
//     {
//         entry = iter._entry;
//         if (entry->_lesser != NULL)
//             txtfile->formatLine("\"node%d\":f0 -> \"node%d\":f1;", index, get_index(entry->_lesser, dict));
//         if (entry->_greater != NULL)
//             txtfile->formatLine("\"node%d\":f2 -> \"node%d\":f1;", index, get_index(entry->_greater, dict));
//         ++iter;
//         ++index;
//     }
// 
//     txtfile->writeLine("}");
//     txtfile->close();
//     
//     System::Run(String::Format("/opt/local/bin/dot -Tpng graph.dot -o %s", filename.text()));
//     
//     Console::writeLine("done");
// }

bool DictTest::basicTest()
{
    Util::Dictionary<int, String> simple_dict;


    simple_dict.add(5, "five"); // 0
    TEST_ASSERT(simple_dict.hasKey(5))
    TEST_ASSERT(simple_dict.containsValue("five"))
    simple_dict.add(4, "four"); // 1
    simple_dict.add(3, "three"); // 2
    simple_dict.add(2, "two"); // 3
    simple_dict.add(1, "one");
    simple_dict.add(0, "zero"); // 5
    simple_dict.add(50, "fifty");
    simple_dict.add(25, "twenty-five");
    simple_dict.add(30, "thirty");
    simple_dict.add(12, "twelve"); 
    simple_dict.add(26, "twenty-six");
    simple_dict.add(24, "twenty-four");
    simple_dict.add(66, "sixty-six");
    simple_dict.add(84, "eighty-four");
    TEST_ASSERT(simple_dict.hasKey(25))
    TEST_ASSERT(simple_dict.containsValue("twenty-five"))
    simple_dict.add(96, "ninety-six");
    simple_dict.add(44, "fourty-four");   
    simple_dict.add(46, "fourty-six");
    simple_dict.add(54, "fifty-four");
    simple_dict.add(36, "thirty-six");
    simple_dict.add(74, "seventy-four");            
    simple_dict.add(59, "fifty-nine");            
    simple_dict.add(72, "seventy-two");            
    simple_dict.add(62, "sixty-two");            
    simple_dict.add(91, "ninety-one");            
    simple_dict.add(88, "eighty-eight");            
    simple_dict.add(63, "sixty-three");            
    simple_dict.add(41, "seventy-four");
    
    TEST_ASSERT(simple_dict[46] == "fourty-six")           
    TEST_ASSERT(simple_dict.size() == 27)
    simple_dict.remove(74);
    simple_dict.remove(84);
    TEST_ASSERT(simple_dict.size() == 25);    
    simple_dict.remove(30);
    simple_dict.remove(25);
    simple_dict.remove(66);
    simple_dict.remove(4);
    TEST_ASSERT(simple_dict.size() == 21);
    simple_dict.add(25, "twenty-five");
    TEST_ASSERT(simple_dict.size() == 22);
    simple_dict.clear();

    // now lets do pointers
    String name1 = "Bob Jones";
    String name2 = "Marco Gomez";
    String name3 = "Richard Lump";
    String name4 = "Isabella Desilva";
    Util::Dictionary<String, String*> ptr_list;
    ptr_list.add("bob", &name1);
    ptr_list.add("marco", &name2);
    ptr_list["richard"] = &name3;
    ptr_list.add("isabella", &name4);
    TEST_ASSERT(ptr_list.size() == 4)
    TEST_ASSERT((*ptr_list["marco"]) == "Marco Gomez")
    name2 = "Mario Anders";
    TEST_ASSERT(*ptr_list.get("marco") == "Mario Anders")
    ptr_list.remove("marco");
    TEST_ASSERT((*ptr_list["richard"]) == "Richard Lump")
    
    Math::Random random;
    // now lets create 1000 items
    uint32 counter = 0;
    uint32 first_value = 0;
    for (int i=0;i<1000;++i)
    {
        uint32 value = random.randomInt(999);
        if (!simple_dict.hasKey(value))
        {
            if (first_value == 0)
                first_value = value;
            ++counter;
            simple_dict.add(value, String::Val(value));
        }
    }
    TEST_ASSERT(simple_dict.size() == counter)
    TEST_ASSERT(simple_dict[first_value] == String::Val(first_value))
    for (uint32 i=0;i<1000;++i)
    {
        if (simple_dict.hasKey(i))
        {
            simple_dict.remove(i);
        }
    }
    TEST_ASSERT(simple_dict.isEmpty())
    
    return true;
}

bool DictTest::iterationTest()
{
    TEST_ASSERT(false)
    return true;
}

bool DictTest::sortTest()
{
    TEST_ASSERT(false)
    return true;
}

bool DictTest::listTest()
{
    TEST_ASSERT(false)
    return true;
}


