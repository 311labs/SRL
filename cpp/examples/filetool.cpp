
#include "srl/app/ArgumentParser.h"
#include "srl/Console.h"
#include "srl/fs/Directory.h"
#include "srl/util/Base64.h"
#include "srl/xml/XML.h"
#include "srl/rpc/RpcValue.h"
#include "srl/sys/System.h"

#include "srl/sys/Keyboard.h"
#include "srl/fs/TextFile.h"
#include "srl/util/Vector.h"
#include "srl/DateTime.h"
#include <stdio.h>

using namespace SRL;
using namespace SRL::FS;

class KeyboardListener : public System::Keyboard::Listener
{
public:
	void hotkey_event_down(const uint32& id)
	{
		Console::formatLine("hotkey down %u", id);
	}

	void hotkey_event_up(const uint32& id)
	{
		Console::formatLine("hotkey up %u", id);
	}
};


void keyboard_handler(const String& path)
{
	System::Keyboard *handler = System::Keyboard::GetInstance();
	KeyboardListener *listener = new KeyboardListener();
	handler->addListener(listener);
	handler->registerHotkey(System::Keyboard::MODKEY_CTL, (int)'A', false);
	System::Sleep(1000);
	handler->registerHotkey(System::Keyboard::MODKEY_CTL+System::Keyboard::MODKEY_SHIFT, (int)'B');

	while (true)
	{
		System::Sleep(1000);
	}
}

void list_dir(const String& path)
{
	FS::Directory etc_dir(path);
	Console::formatLine("total %u", etc_dir.totalChildren());
	for (uint32 i=0; i<etc_dir.totalChildren();++i)
	{
	    FsObject *fsobj = etc_dir.getChild(i);
	    String perms = "-";
	    if (fsobj->isDir())
	        perms = "d";
	    perms += fsobj->permissions().asString();
	    Console::formatLine("%s %s:%-8s %8s %8s %s", perms.text(),
	        FS::GetUserFromUID(fsobj->uid()).text(),FS::GetGroupFromGID(fsobj->gid()).text(),
	        String::Val(fsobj->size()).text(), fsobj->modifiedDate().formatString("%b %d %Y %H:%M").text(), 
	        fsobj->name().text());
	}    
}

void encode1(const String& path)
{
    String str = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
    String encoded = Base64::encode(str);
    Console::write("encoding string:\n");
    Console::write(str);
    Console::write("\nresult=\n");
    Console::write(encoded);
    Console::write("\n");
    Console::write("\ndecoded=\n");
    String decoded = Base64::decode(encoded);
    Console::write(decoded);
    Console::write("\n");
    
}

void encode2(const String& path)
{
    String str = "If 1 > 2 and 3 < 2 then what is 50% of 2?  If 'he' says then \"she says\", what is next?";
    String encoded = XML::encode(str);
    Console::write("encoding string:\n");
    Console::write(str);
    Console::write("\nresult=\n");
    Console::write(encoded);
    Console::write("\n");
    Console::write("\ndecoded=\n");
    String decoded = XML::decode(encoded);
    Console::write(decoded);
    Console::write("\n");
    
}

void parseXML(const String& xml, int* offset)
{
    String tag = XML::getNextTag(xml, offset);
    if (tag == "")
        return;
        
    Console::write("found tag: ");
    Console::writeLine(tag);
    
    int n = *offset;
    String ntag = XML::getNextTag(xml, &n);    
    if (ntag == "")
    {
        Console::write("\tdata: ");
        Console::writeLine(XML::parseTag(tag.c_str(), xml, &n));
    }
    else
        parseXML(xml, offset);
    
    tag.insert(1, '/');
    if (XML::findTag(tag.c_str(), xml, offset))
    {
        Console::write("end tag: ");
        Console::writeLine(tag);
    }
    else
    {
        Console::write("no end tag: ");
        Console::writeLine(tag);
    }
    
    parseXML(xml, offset);
}


void encode(const String& path)
{
    String enc = "abs!$&'()*+,%%;=:@/?";
    char c = '\0';
    String hex(4);
    for(uint32 i = 0; i < enc.length(); ++i)
    {
        hex.clear();
        String::Hex(enc[i], hex);
        Console::formatLine("char '%c' has a hex of '0x%s' which becomes '%c'", 
            enc[i], hex.text(), String::UnHex(hex[0], hex[1]));
    }
    
    
}

void encode3(const String& path)
{
    // // lets try and parse an XML file
    // XML::Parser parser(FS::Cat(path));
    // RPC::RpcValue *value = NULL;
    // 
    // while (parser.nextTag())
    // {
    //     if (parser.currentTag() == "value")
    //     {
    //         Console::writeLine("+ start tag: value");
    //         if (value != NULL)
    //             delete value;
    //         
    //         value = RPC::RpcValue::FromXml(parser);
    //         
    //         if (value->getType() == RPC::RpcValue::TypeString)
    //         {
    //             Console::write("\tstring = ");
    //             Console::writeLine(value->asString());
    //         }
    //         else if (value->getType() == RPC::RpcValue::TypeStruct)
    //         {
    //             RPC::RpcStructValue *struct_value = (RPC::RpcStructValue*)value;
    //             
    //             Console::writeLine("\tstruct =");
    //             if (struct_value->hasMember("faultCode"))
    //             {
    //                 Console::formatLine("\t\tfault code: %d", struct_value->get("faultCode")->asInt());
    //             }
    //             
    //             if (struct_value->hasMember("faultString"))
    //             {
    //                 Console::formatLine("\t\tfault string: %s", struct_value->get("faultString")->asString().text());
    //             }
    //         }
    //         else if (value->getType() == RPC::RpcValue::TypeInt)
    //         {
    //             Console::write("\tint = ");
    //             Console::formatLine("%d", value->asInt());
    //         }
    //         else if (value->getType() == RPC::RpcValue::TypeDateTime)
    //         {
    //             Console::write("\tdatetime = ");
    //             Console::writeLine(value->asDateTime().asString());
    //         }
    //         else if (value->getType() == RPC::RpcValue::TypeBase64)
    //         {
    //             RPC::RpcBinaryValue *binary_value = (RPC::RpcBinaryValue*)value;
    //             Console::write("\tbinary = ");
    //             Console::writeLine(binary_value->bytes().c_str());
    //         }            
    //         else if (value->getType() == RPC::RpcValue::TypeBoolean)
    //         {
    //             Console::write("\tbool = ");
    //             if (value->asBool())
    //                 Console::writeLine("True");
    //             else
    //                 Console::writeLine("False");
    //         }
    //         else if (value->getType() == RPC::RpcValue::TypeDouble)
    //         {
    //             Console::write("\tdouble = ");
    //             Console::formatLine("%f", value->asDouble());
    //         }      
    //         else if (value->getType() == RPC::RpcValue::TypeArray)
    //         {
    //             RPC::RpcArrayValue *array_value = (RPC::RpcArrayValue*)value;
    //             Console::formatLine("\tarray(%d values)", array_value->size());
    //             for(int i = 0; i < array_value->size(); ++i)
    //             {
    //                 Console::formatLine("\tvalue type: %d\t\tvalue: %s", array_value->get(i)->getType(),
    //                     array_value->get(i)->asString().text());
    //             }
    //         }      
    //         else
    //         {
    //             Console::formatLine("invalid value type: %d", value->getType());    
    //         }
    //         
    //     }
    //     
    //     if (parser.isClosingTag())
    //     {
    //         Console::write("- end tag: ");
    //         Console::writeLine(parser.currentTag());            
    //     }
    //     else
    //     {
    //         Console::write("+ start tag: ");
    //         Console::writeLine(parser.currentTag());
    //         if (parser.hasText())
    //         {
    //             Console::write("\ttag value: ");
    //             Console::writeLine(parser.getText());
    //         }
    //     }
    // }
    // // STRUCT RPC VALUE
    // RPC::RpcStructValue rpc_struct;
    // rpc_struct.set("name", "Ian Starnes");
    // rpc_struct.set("age", "32");
    // rpc_struct.set("weight", "164.25");
    // Console::writeLine(rpc_struct.asXml());
    // 
    // 
    // rpc_value.clear();
    // rpc_value = true;
    // rpc_struct['is_open'] = rpc_value;
    // 
    // Console::write("1");
    // rpc_value.clear();
    // rpc_value = 123.12345;
    // rpc_struct['temperature'] = rpc_value;
    // 
    //     Console::write("3");
    // DateTime dt;
    // rpc_value.clear();
    // rpc_value = dt;
    // rpc_struct['date'] = rpc_value;    
    // 
    //     Console::write("4");
    // Console::writeLine(rpc_struct.toXml());
    
}

void decode(const String& path)
{
    String str = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
    String encoded = Base64::encode(str);
    Console::write("encoding string:\n");
    Console::write(str);
    Console::write("\nresult=\n");
    Console::write(encoded);
    Console::write("\n");    
}




int main (int argc, char const* argv[])
{
    App::ArgumentParser argparser("SRL File System Examples", "This is an example of how to use SRL FS Logic", "0.0.0");
    argparser.add("verbose", "-v, --verbose", "display verbose output");
    argparser.add("help", "--help", "display this output");
    argparser.add("list", "-l, --list", "list the file/directory information");
    argparser.add("encode", "--encode", "encode a file using base64");
    argparser.add("decode", "--decode", "decode a file using base64");
	argparser.add("keyboard", "-k, --keyboard", "list the file/directory information");
    argparser.parse(argc, argv);
    FS::Permissions perms(FS::READ_WRITE_EXECUTE, FS::READ_WRITE_EXECUTE, FS::READ_WRITE_EXECUTE);

    Console::format("%0.0f\n", System::GetSystemTimer());

    DateTime dt;
    printf("%s\n", dt.asString().text());

    TimeValue tv;
    System::GetCurrentTime(tv);
            const time_t t = tv.tv_sec;
            tm *lt = localtime(&t);
            tv.tv_dst = lt->tm_isdst;
            tv.tv_sec -= (tv.tv_zone*3600) - (tv.tv_dst ? 3600 : 0);
	
	Console::format("dst: %d  tz: %d\n", tv.tv_dst, tv.tv_zone);

    return 1;
     
    if (argparser.get("list")->wasParsed() && argparser.getArgCount())
    {
        list_dir(argparser.getArgs()[0]);
        return 1;
    }

    if (argparser.get("encode")->wasParsed() && argparser.getArgCount())
    {
        encode(argparser.getArgs()[0]);
        return 1;
    }

    if (argparser.get("decode")->wasParsed() && argparser.getArgCount())
    {
        decode(argparser.getArgs()[0]);
        return 1;
    }

    if (argparser.get("keyboard")->wasParsed() && argparser.getArgCount())
    {
        keyboard_handler(argparser.getArgs()[0]);
        return 1;
    }


    //Time t;
    // printf("%s\n", t.asString().text());
    // t.update(); System::Sleep(700);
    // printf("%s\n", t.asString().text());
    // t.update(); System::Sleep(700);
    // printf("%s\n", t.asString().text());
    // t.update(); System::Sleep(700);
    // printf("%s\n", t.asString().text());
    
    SRL::Util::Vector<int> my_nums;
    
    for(int i=0; i<10; ++i)
    {
        my_nums.add(i);
    }

    Util::Vector<int>::Iterator iter = my_nums.begin();
    for (; iter!=my_nums.end(); ++iter)
    {
        printf("value='%d'\n", iter.value());
        if (iter.value() == 4)
        {
            iter = my_nums.erase(iter);
            --iter;
        }
    }
    
    iter = my_nums.begin();
    for (; iter!=my_nums.end(); ++iter)
    {
        printf("value='%d'\n", iter.value());
    }
    
    

    argparser.printHelp();
    return 0;
}
