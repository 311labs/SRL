#include "StringTest.h"

#include <string.h>

using namespace SRL;
using namespace SRL::Test;

bool StringTest::construct()
{
    // test string construction
    // empty string
    String str_empty;
    TEST_ASSERT(str_empty.isEmpty())
    // string from c string
    String str_cstr1("hello world");
    TEST_ASSERT(strcmp(str_cstr1.text(), "hello world")==0)
    String str_cstr2("hello world", 5);
    TEST_ASSERT(strcmp(str_cstr2.text(), "hello")==0)
    // two c strings together
    String str_two("hello", " world");
    TEST_ASSERT(strcmp(str_two.text(), "hello world")==0)
    // create a string filled with a characeter
    String str_fill('x', 10);
    TEST_ASSERT(strcmp(str_fill.text(), "xxxxxxxxxx")==0);
    
    return true;
}

bool StringTest::comparison()
{
    // lets check comparison operations so we can use these later on
    String my_str1("Ape vs Man");
    std::string stl_str1("Ape vs Man");
    const char* c_str1 = "Ape vs Man";
    char* cn_str1 = new char[32];
    strcpy(cn_str1, c_str1);
    // our not equal set
    String my_str2("Gorilla vs Man");
    std::string stl_str2("Gorilla vs Man");
    const char* c_str2 = "Gorilla vs Man";
    const char* c_str3 = "goriLLa vs mAn";
    String my_str3("gorilla vs man");
    
    // lets test equality
    // c style first 
    TEST_ASSERT(my_str1 == c_str1);
    TEST_ASSERT(my_str1 == cn_str1);
    TEST_ASSERT(c_str1 == my_str1);
    TEST_ASSERT(cn_str1 == my_str1);
    // STL String
    TEST_ASSERT(my_str1 == stl_str1);
    TEST_ASSERT(stl_str1 == my_str1);
    // Our String
    TEST_ASSERT(my_str1 == my_str1);
    
    // ignore case comparison
    TEST_ASSERT(my_str2.isEqualTo(c_str2, false));
    TEST_ASSERT(!my_str2.isEqualTo(c_str3, false));
    TEST_ASSERT(my_str2.isEqualTo(c_str3, true));
    TEST_ASSERT(my_str2.isEqualTo(my_str3, true));
    
    // now fail equal
    // c style first 
    TEST_ASSERT(!(my_str1 == c_str2));
    // STL String
    TEST_ASSERT(!(my_str1 == stl_str2));
    TEST_ASSERT(!(stl_str1 == my_str2));
    // Our String
    TEST_ASSERT(!(my_str1 == my_str2));

    // now test not equal
    // c style first 
    TEST_ASSERT(my_str1 != c_str2);
    TEST_ASSERT(c_str2 != my_str1);
    // STL String
    TEST_ASSERT(my_str1 != stl_str2);
    TEST_ASSERT(stl_str2 != my_str1);
    // Our String
    TEST_ASSERT(my_str1 != my_str2);

    // now fail equal
    // c style first 
    TEST_ASSERT(!(my_str1 != c_str1));
    TEST_ASSERT(!(c_str1 != my_str1));
    // STL String
    TEST_ASSERT(!(my_str1 != stl_str1));
    TEST_ASSERT(!(stl_str1 != my_str1));
    // Our String
    TEST_ASSERT(!(my_str1 != my_str1));

    delete[] cn_str1;
    
    // begins with
    String beginEnd_test = "Don't worry be happy";
    TEST_ASSERT(beginEnd_test.beginsWith("Don't"))
    TEST_ASSERT(!beginEnd_test.beginsWith("on't"))
    TEST_ASSERT(beginEnd_test.endsWith("be happy"))
    TEST_ASSERT(!beginEnd_test.endsWith("be happ")) 
    
    my_str1.clear();
    my_str2.clear();
    stl_str1.clear();
    stl_str2.clear();
    return true;
}


bool StringTest::assignment()
{
    String str;
    TEST_ASSERT(str.isEmpty());
    // first lets create a string from a c string
    str = "hello world";
    TEST_ASSERT(strcmp(str.text(), "hello world")==0);
    // std::string
    std::string stl = "hello stl";
    str = stl;
    TEST_ASSERT(strcmp(str.text(), "hello stl")==0);
    // another String 
    String astr("Monkey Dance");
    str = astr;
    TEST_ASSERT(strcmp(str.text(), "Monkey Dance")==0);

    // combine strings
    String my_str("Lets do");
    std::string stl_str(" the monster dance?");
    const char* c_str = " How about the tango?";
    char* cn_str = new char[64];
    strcpy(cn_str, " NO THE SALSA!");

    my_str += stl_str;
    TEST_ASSERT(my_str == "Lets do the monster dance?");
    my_str += c_str;
    TEST_ASSERT(my_str == "Lets do the monster dance? How about the tango?");
    my_str += cn_str;
    TEST_ASSERT(my_str == "Lets do the monster dance? How about the tango? NO THE SALSA!");

    // lets assign by position
    my_str = "12x45";
    TEST_ASSERT(my_str[2] = 'x');
    my_str[2] = '3';
    TEST_ASSERT(my_str == "12345");

    // now lets create a string of a fixed size
    my_str.clear();
    TEST_ASSERT(my_str == "");
    my_str.resize(4);
    my_str[0] = '1';    my_str[1] = '2';
    my_str[2] = '3';    my_str[3] = '4';
    TEST_ASSERT(my_str == "1234");

    // now lets play with adding strings together
    my_str = "Why are you looking at my code?";
    strcpy(cn_str, "\nBecause I am admiring your code!");
    String answer = my_str + cn_str;
    TEST_ASSERT(answer == "Why are you looking at my code?\nBecause I am admiring your code!");

    const char* const_str = "\nBecause I am a loser and like to pick apart other peoples code!";
    answer = my_str + const_str;
    TEST_ASSERT(answer == "Why are you looking at my code?\nBecause I am a loser and like to pick apart other peoples code!");

    stl = "\nThe meaning of life exists encrypted in this code!";
    answer = my_str + stl;
    TEST_ASSERT(answer == "Why are you looking at my code?\nThe meaning of life exists encrypted in this code!");


    String wise_man = "\nThe meaning of life exists encrypted in this code!";
    answer = my_str + wise_man;
    TEST_ASSERT(answer == "Why are you looking at my code?\nThe meaning of life exists encrypted in this code!");

    my_str = "\nI heard it was Shane!";
    const char* const_str2 = "Who shot the sherrif?";
    answer = const_str2 + my_str;
    TEST_ASSERT(answer == "Who shot the sherrif?\nI heard it was Shane!");

    my_str = "\nI heard it was Shane!";
    stl = "Who crossed the road with the chicken?";
    answer = stl + my_str;
    TEST_ASSERT(answer == "Who crossed the road with the chicken?\nI heard it was Shane!");
    
    delete[] cn_str;
    return true;
}

bool StringTest::access()
{
    String my_str("hello");
    TEST_ASSERT(strlen(my_str.text())==5);
    TEST_ASSERT(my_str.length()==5);

    // get substring
    my_str = "world peace and food for all";
    TEST_ASSERT(my_str.substr(6, 5) == "peace");
    TEST_ASSERT(my_str.substr(16) == "food for all");
    TEST_ASSERT(my_str.substr(0, 5) == "world");
    return true;
}

bool StringTest::conversion()
{
    // trim whitespaces
    String my_str("    hello    ");
    my_str.trimBegin();
    TEST_ASSERT(my_str == "hello    ");
    my_str.trimEnd();
    TEST_ASSERT(my_str == "hello");
    my_str = "    hello    ";
    my_str.trim();
    TEST_ASSERT(my_str == "hello");

    // change case
    my_str.toUpper();
    TEST_ASSERT(my_str == "HELLO");
    my_str.toLower();
    TEST_ASSERT(my_str == "hello");

    // truncate
    my_str.truncate(4);
    TEST_ASSERT(my_str == "hell");

    // clear
    my_str.clear();
    TEST_ASSERT(my_str.isEmpty());
    TEST_ASSERT(my_str == "");

    // prepend
    my_str = "345";
    my_str.prepend("12");
    TEST_ASSERT(my_str == "12345");

    my_str = "345";
    String pre_str = "12";
    my_str.prepend(pre_str);
    TEST_ASSERT(my_str == "12345");

    my_str = "345";
    my_str.prepend('2');
    my_str.prepend('1');
    TEST_ASSERT(my_str == "12345");

    // insert
    my_str = "125";
    String ins_str = "34";
    my_str.insert(2,ins_str);
    TEST_ASSERT(my_str == "12345");

    my_str = "125";
    my_str.insert(2,"34");
    TEST_ASSERT(my_str == "12345");

    // replace
    my_str = "Goolaa Goolaa How many cows?";
    String rep_str = "Hoya";
    my_str.replace("Goolaa",rep_str);
    TEST_ASSERT(my_str == "Hoya Hoya How many cows?");

    my_str = "Goolaa Goolaa How many cows?";
    my_str.replace("Goolaa","Kaka");
    TEST_ASSERT(my_str == "Kaka Kaka How many cows?");

    my_str = "Goolaa Goolaa How many cows?";
    my_str.replace('a','e');
    TEST_ASSERT(my_str == "Goolee Goolee How meny cows?");

    my_str = "Goolaa Goolaa How many cows?";
    my_str.replaceFrom(7, 6, "Bees");
    TEST_ASSERT(my_str == "Goolaa Bees How many cows?");

    // remove
    my_str = "Goolaa Goolaa How many cows?";
    my_str.remove("Goolaa ");
    TEST_ASSERT(my_str == "How many cows?");    

    my_str = "Goolaa Goolaa How many cows?";
    my_str.removeChar('l');
    TEST_ASSERT(my_str == "Gooaa Gooaa How many cows?");    

    // let play with format string
    // local format
    String for_str = my_str.format("my age is %d   and i am %0.2f", 18, 6.22341f);
    TEST_ASSERT(my_str == "my age is 18   and i am 6.22");
    // global format
    for_str = String::Format("So check it out... I went %0.4f meters... then %d feet... then said %s",
                             5.5555111, 5000, "hello my monkey girl");
    TEST_ASSERT(for_str == "So check it out... I went 5.5555 meters... then 5000 feet... then said hello my monkey girl");

    // number conversion
    // number to string
    int32 inum = -55555; uint32 uinum=55555; 
    float32 fnum = 55.555f; float64 dnum = 55555.55555;
    TEST_ASSERT(String::Val(inum) == "-55555");
    TEST_ASSERT(String::Val(uinum) == "55555");
    TEST_ASSERT(String::Val(fnum,3) == "55.555");
    TEST_ASSERT(String::Val(dnum,5) == "55555.55555");

    // string to number
    my_str = "-1975";
    TEST_ASSERT(String::Int(my_str)==-1975);
    my_str = "1975";
    TEST_ASSERT(String::UInt(my_str)==1975);
    my_str = "19.75";
    TEST_ASSERT(String::Float(my_str)==19.75f);
    my_str = "1975.1975111";
    TEST_ASSERT(String::Double(my_str)==1975.1975111);

    // Hex
    String enc = "ABZde156!$&'()*+,%%;=:@/?";
    String hex(4);
    for(uint32 i = 0; i < enc.length(); ++i)
    {
        hex.clear();
        String::Hex(enc[i], hex);
        TEST_ASSERT(enc[i] == String::UnHex(hex[0], hex[1]));
    }

    return true;
}

bool StringTest::search()
{
    String my_str = "The first definite and tangible proofs of soap making are found in the history of ancient Rome. Pliny, the Roman historian, described soap being made from goat's tallow and causticized wood ashes. He also wrote of common salt being added to make the soap hard. The ruins at Pompeii revealed a soap factory complete with finished bars. \
While the Romans are well known for their public baths, generally soap was not used for personal cleaning. To clean the body the Greeks and then the Romans would rub the body with olive oil and sand. A scraper, called a strigil, was then used to scrape off the sand and olive oil also removing dirt, grease, and dead cells from the skin leaving it clean. Afterwards the skin was rubbed down with salves prepared from herbs. \
Throughout history people were also known to take baths in herb waters and other additions to the bathing medium thought to be beneficial. It is well known that Cleopatra, who captivated the leaders of the Roman world, attributed her beauty to her baths in mare's milk. During the early century of the common era soap was used by physicians in the treatment of disease. Galen, a 2nd century physician, recommended bathing with \
soap would be beneficial for some skin conditions. Soap for personal washing became popular during the later centuries of the Roman era. \
The Celtic peoples are also though by some historians to have discovered soap making and were using it for bathing and washing. Maybe do to \
increased contact with the Celtics by the Romans, using soap for personal washing care became popular. \
It is also important to remember when writing a history of life styles there are no grand trends that get disseminated";
    my_str += "throughout the globe via mass communications such as we have today. Usage and knowledge of common skills \
and arts can vary from one locale to the next. When they are starting to use soap in the public \
baths in 3rd century A.D. in the major cities, the people in small villages are likely to be using \
the olive oil, sand, and strigil method. The Celtics might have been washing their faces daily with \
soap long before the Romans even went over the Italian Alps. Dates can be exact when dealing with events \
such as battles, births, and deaths. But not usage of every day items. 'The state of the art' varies \
depending both on time and location.";
    // time to find
    String smp = "123, 678, 1234";
    TEST_ASSERT(smp.rfind(',') == 8);

    TEST_ASSERT(my_str.find(',') == 101);
    TEST_ASSERT(my_str.find(',', 102) == 122);
    TEST_ASSERT(my_str.rfind(',') == 2212);
    TEST_ASSERT(my_str.rfind(',', 120) == 101);

    TEST_ASSERT(my_str.find("Pliny") == 96);
    TEST_ASSERT(my_str.rfind("Pliny", 150) == 96);    

    
    // lets count the number of times the letter a appears in our text
    TEST_ASSERT(my_str.count("Romans")==4);
    String btw = my_str.between('.', '.');
    TEST_ASSERT(btw == " Pliny, the Roman historian, described soap being made from goat's tallow and causticized wood ashes");
    btw = my_str.between('.', ',');
    TEST_ASSERT(btw == " Pliny");
    btw = my_str.between('.','.',2);
    TEST_ASSERT(btw == " He also wrote of common salt being added to make the soap hard");
    
    btw = my_str.between("Romans", "soap");
    TEST_ASSERT(btw == " are well known for their public baths, generally ");
    btw = my_str.between("Romans", "soap", 3);
    TEST_ASSERT(btw == ", using ");
    
    btw = smp.between('.', '.');
    TEST_ASSERT(btw.isEmpty());
    
    // now lets check out our sections
    btw = my_str.section(' ',0);
    TEST_ASSERT(btw == "The");
    btw = my_str.section(' ',4);
    TEST_ASSERT(btw == "tangible");
    String test = "Hello Hi Hey Yo";
    btw = test.section(' ', 4);
    TEST_ASSERT(btw.isEmpty());

    // string delimeter secionds
    test = "field1, field2, field3, field4";
    btw = test.section(", ", 0);
    TEST_ASSERT(btw == "field1");
    btw = test.section(", ", 2);
    TEST_ASSERT(btw == "field3");
    btw = test.section(", ", 4);
    TEST_ASSERT(btw == "");

    // get left characters
    test = "1234567890";
    TEST_ASSERT(test.left(3) == "123");
    TEST_ASSERT(test.mid(4, 4) == "5678");
    TEST_ASSERT(test.right(3) == "890");
    return true;
}

bool StringTest::std_interface()
{
    String my_str = "1234567890";
    TEST_ASSERT(my_str.size() == 10);
    TEST_ASSERT(strcmp(my_str.c_str(),"1234567890")==0);
    return true;
}
