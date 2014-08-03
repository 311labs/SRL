#include "srl/app/ArgumentParser.h"
#include "srl/Console.h"

#include <stdlib.h>

using namespace SRL;
using namespace SRL::App;

Argument::Argument(const String& name, const String& keys, const String& help) :
_name(name), _keys(NULL),
_key_count(0), _help(help),
_value(),
_has_value(false),
_was_parsed(false) 
{
    parseKeys(keys);
}

Argument::Argument(const String& name, const String& keys, const String& help, const String& default_value) :
_name(name), _keys(NULL),
_key_count(0), _help(help),
_value(default_value),
_has_value(true),
_was_parsed(false)
{
    parseKeys(keys);
}

void Argument::parseKeys(const String& keys)
{
    if (_keys != NULL)
        delete[] _keys;
    
    _key_count = keys.count(',') + 1;
    _keys = new String[_key_count];
    for (int i=0; i < _key_count; ++i)
    {
        String key = keys.section(',', i);
        _keys[i] = key.trim();
    }
    
}

Argument::~Argument()
{
    if (_keys != NULL)
    {
        _key_count = 0;
        delete[] _keys;    
    }
}

bool Argument::hasKey(const String& key)
{
    for (int i=0;i<_key_count;++i)
    {
        if (_keys[i] == key)
            return true;
    }
    return false;
}

bool Argument::getBoolValue() const
{
	if (_value == "true")
		return true;
	else if (_value == "false")
		return false;
	else if (_value.isDigits())
	{
		return (getIntValue() != 0);
	}
	return false;
}


// ARGUMENT PARSER ============
ArgumentParser::ArgumentParser(const String& title, const String& description,
const String& version) : 
_args(NULL), _title(title), _description(description), _version(version)
{
    
}

ArgumentParser::~ArgumentParser()
{
    Util::Dictionary<String, Argument*>::Iterator iter = _arguments.begin();
    for (; iter != _arguments.end(); ++iter)
    {
        delete iter.value();
    }
    
    if (_args != NULL)
    {
        delete[] _args;
    }
}

void ArgumentParser::parse(int argc, const char* argv[])
{
    if (_args != NULL)
    {
        delete[] _args;
    }
    // FIXME parse multiple options in a single argument -vqp
    String help_str = "--help";
    String shelp_str = "-h";
    
    _args = new String[argc];
    _arg_count = 0;    
    _program = argv[0];
    for (int i = 1; i < argc; ++i)
    {
        // if argv is help then just dump help
        if ((help_str == argv[i]) || (shelp_str == argv[i]))
        {
            printHelp();
            exit(1);
        }
        
        Argument *arg = findArgByKey(argv[i]);
        if (arg != NULL)
        {
            arg->setParsed();
            if (arg->hasValue())
            {
                ++i;
                arg->setValue(argv[i]);
            }
        }
        else
        {
            _args[_arg_count] = argv[i];
            ++_arg_count;
        }
    }
}

Argument* ArgumentParser::findArgByKey(const String& key)
{
    Util::Dictionary<String, Argument*>::Iterator iter = _arguments.begin();
    for (; iter != _arguments.end(); ++iter)
    {
        if (iter.value()->hasKey(key))
            return iter.value();
    }
    return NULL;
}

void ArgumentParser::add(const String& name, const String& keys, const String& help)
{
    Argument *arg = new Argument(name, keys, help);
    _arguments.add(name, arg);
}

void ArgumentParser::add(const String& name, const String& keys, 
         const String& help, const String& default_value)
{
    Argument *arg = new Argument(name, keys, help, default_value);
    _arguments.add(name, arg);
}

void ArgumentParser::printHelp()
{
    Console::write(_title);
    if (_version.length()>0)
    {
        Console::write("\t");
        Console::write(_version);
    }
    Console::writeLine("");
    Console::writeLine(_description);
    
    Util::Dictionary<String, Argument*>::Iterator iter = _arguments.begin();
    for (; iter != _arguments.end(); ++iter)
    {
        Console::write("\t");
        Argument* arg = iter.value();
        for (int k=0;k<arg->getKeyCount();++k)
        {
            Console::write(arg->getKeys()[k]);
            Console::write(" ");
        }
        Console::write("\t");
        Console::writeLine(arg->getHelp());
    }   
    
}

