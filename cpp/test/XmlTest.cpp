#include "XmlTest.h"
#include "srl/fs/TextFile.h"
#include "srl/xml/XML.h"

using namespace SRL;
using namespace SRL::Test;

bool XmlTest::parserTest()
{
    // lets try and find out xml file path
    String xml_path = "cpp/test";
    if (!FS::Exists(xml_path + "/simple.xml"))
    {
        xml_path = "test";
        if (!FS::Exists(xml_path + "/simple.xml"))
        {
            xml_path = "";
            if (!FS::Exists("simple.xml"))
            {
                throw Errors::Exception(String::Format("could not find xml files: '%s'", FS::GetCurrentDir().text()));
            }
        }
    }
    FS::TextFile *xml_file = FS::OpenTextFile(xml_path + "/simple.xml");
    XML::Parser parser(xml_file);
    XML::Parser::Element *element = parser.getNextElement();
    TEST_ASSERT(element->name() == "xml");
    TEST_ASSERT(element->hasAttributes());
    XML::Parser::Attribute* attr = element->getNextAttribute();
    TEST_ASSERT(attr->name() == "version");
    TEST_ASSERT(attr->value() == "1.0");
    element->getNextAttribute();
    TEST_ASSERT(attr->name() == "encoding");
    TEST_ASSERT(attr->value() == "ISO-8859-1");
    
    parser.getNextElement();
    TEST_ASSERT(element->name() == "breakfast_menu");
    TEST_ASSERT(element->isClosing() == false);
    attr = element->getNextAttribute();
    TEST_ASSERT(attr->name() == "size");
    TEST_ASSERT(attr->value() == "5");    
    
    parser.getNextElement();
    TEST_ASSERT(element->name() == "food");
    TEST_ASSERT(element->hasContent() == false);    
    
    parser.getNextElement();
    TEST_ASSERT(element->name() == "name");
    TEST_ASSERT(element->isClosing() == false);
    TEST_ASSERT(element->hasContent());
    TEST_ASSERT(element->getContent() == "Belgian Waffles");
    
    parser.getNextElement();
    TEST_ASSERT(element->name() == "name");
    TEST_ASSERT(element->isClosing() == true);
    TEST_ASSERT(element->hasContent() == false);
    
    // now lets test the findElement
    element = parser.findNextElement("name");
    TEST_ASSERT(element->name() == "name")
    TEST_ASSERT(element->isClosing() == false);
    TEST_ASSERT(element->hasContent());
    TEST_ASSERT(element->getContent() == "Strawberry Belgian Waffles");
    
    element = parser.findNextElement("name");
    TEST_ASSERT(element->name() == "name")
    TEST_ASSERT(element->isClosing() == false);
    TEST_ASSERT(element->hasContent());
    TEST_ASSERT(element->getContent() == "Berry-Berry Belgian Waffles");    

    element = parser.findNextElement("name");
    TEST_ASSERT(element->name() == "name")
    TEST_ASSERT(element->isClosing() == false);
    TEST_ASSERT(element->hasContent());
    TEST_ASSERT(element->getContent() == "French Toast");
    
    element = parser.findNextElement("name");
    TEST_ASSERT(element->name() == "name")
    TEST_ASSERT(element->isClosing() == false);
    TEST_ASSERT(element->hasContent());
    TEST_ASSERT(element->getContent() == "Homestyle Breakfast");    

    element = parser.findNextElement("calories");
    TEST_ASSERT(element->name() == "calories")
    TEST_ASSERT(element->isClosing() == false);
    TEST_ASSERT(element->hasContent());
    TEST_ASSERT(element->getContent() == "950");

    element = parser.findPreviousElement("description");
    TEST_ASSERT(element->name() == "description")
    TEST_ASSERT(element->isClosing() == false);
    TEST_ASSERT(element->hasContent());
    TEST_ASSERT(element->getContent() == "two eggs, bacon or sausage, toast, and our ever-popular hash browns");


    return true;
}