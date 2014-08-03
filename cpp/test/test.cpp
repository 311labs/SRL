
#include "srl/Console.h"

#include "TypeTest.h"
#include "SystemTest.h"
#include "StringTest.h"
#include "util/VectorTest.h"
#include "util/DictTest.h"
#include "fs/FsTests.h"
#include "net/SocketTest.h"
#include "net/HttpTests.h"
#include "ThreadTest.h"
#include "XmlTest.h"
#include "JsonTest.h"

#include "srl/app/ArgumentParser.h"

using namespace SRL;
using namespace Test;

int main (int argc, char const* argv[])
{
    int major, minor, revision;
    SRL::GetVersion(major, minor, revision);
    String srl_version = String::Format("%d.%d.%d", major, minor, revision);
    
    App::ArgumentParser argparser("PROGRAM NAME", "DESCRIPTION", srl_version);
    argparser.add("verbose", "-v, --verbose", "display verbose output");
    argparser.add("html", "--html", "output as html");
    argparser.add("list", "-l, --list", "list the unit tests available");
    argparser.add("help", "--help", "display this output");
    argparser.parse(argc, argv);
    
    bool test_all = (argparser.getArgCount() == 0);
    UnitTestManager test(argparser.get("html")->wasParsed());
    
    TypeTest type_tests;
    test.add(&type_tests);
       
    SystemTest sys_tests;
    test.add(&sys_tests);
    
    StringTest str_tests;
    test.add(&str_tests);
    
    VectorTest vector_tests;
    test.add(&vector_tests);
    
    DictTest dict_tests;
    test.add(&dict_tests);
    
    FsTest ts_tests;
    test.add(&ts_tests);

    ThreadTest thread_tests;
    test.add(&thread_tests);

    SocketTest socket_tests;
    test.add(&socket_tests);
    
    HttpTest http_tests;
    test.add(&http_tests);

    XmlTest xml_tests;
    test.add(&xml_tests);
    
    JsonTest json_tests;
    test.add(&json_tests);
    
    if (argparser.get("list")->wasParsed())
    {
        test.listTests();
        return 0;
    }
    
    if (test_all)
    {
        test.run();
        return 0;
    }
    
    test.run(argparser.getArgs(), argparser.getArgCount());
    
    return 0;
}

