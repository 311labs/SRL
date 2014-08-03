#include "JsonTest.h"
#include "srl/fs/TextFile.h"
#include "srl/json/JSON.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace SRL;
using namespace SRL::Test;

bool JsonTest::parserTest()
{   
    std::string teststr(
            "{"
            "  \"foo\" : 1,"
            "  \"bar\" : false,"
            "  \"person\" : {\"name\" : \"GWB\", \"age\" : 60},"
            "  \"data\": [\"abcd\", 42, 54.7]"
            "}"
                   );
    std::istringstream input(teststr);
    JSON::Object o;
    assert(JSON::Object::parse(input, o));
    assert(1 == o.get<double>("foo"));
    assert(o.has<bool>("bar"));
    assert(o.has<JSON::Object>("person"));
    assert(o.get<JSON::Object>("person").has<double>("age"));
    assert(o.has<JSON::Array>("data"));
    assert(o.get<JSON::Array>("data").get<double>(1) == 42);
    assert(o.get<JSON::Array>("data").get<std::string>(0) == "abcd");
    assert(o.get<JSON::Array>("data").get<double>(2) == 54.7);
    assert(!o.has<double>("data"));
    return true;
}