
#include "srl/app/IniParser.h"
#include "srl/fs/TextFile.h"

using namespace SRL;
using namespace SRL::App;

static IniParser *__global_config = NULL;

IniParser& App::GetGlobalConfig()
{
    if (__global_config == NULL)
    {
        __global_config = new IniParser("global.ini");
    }
    return *__global_config;
}


IniParser::IniParser(const String &filename) :
_filename(filename)
{
	reparse();	
}

IniParser::~IniParser()
{
	clear();
}

IniParser::Section* IniParser::getSection(const String& name)
{
	if (!_sections.hasKey(name))
	{
		_sections[name] = new Section(name);
	}
	return _sections[name];
}

void IniParser::clear()
{
	SectionMap::Iterator iter = _sections.begin();
	for (; iter != _sections.end(); ++iter)
	{
		delete iter.value();
	}
	
	_sections.clear();
}

void IniParser::parse(const String& filename)
{
    _filename = filename;
    reparse();
}

void IniParser::reparse()
{
	if (FS::Exists(_filename))
	{
		FS::TextFile *file = FS::OpenTextFile(_filename, false);
		Section *section = getSection("default");
		_sections["default"] = section;
		
		while (!file->eof())
		{
			String line = file->readLine();
			if (line[0] == '[')
			{
				section = getSection(line.between('[', ']'));
				_sections[section->getName()] = section;
			}
			else if ((line[0] == '#') || (line[0] == ';'))
			{
				// comment ignore
			}
			else if (line.count('=') > 0)
			{
				// value lets parse it
				String field = line.section('=', 0).trim();
				String value = line.section('=', 1).trim();
				// remove quotes
				if ((value[0] == '"')||(value[0] =='\''))
				{
					value = value.substr(1, value.length()-2);	
				}
				section->putString(field, value);
			}
			else
			{
				// unknown
				// dump message on screen or throw error or what??
			}
			
			
		}
		file->close();
	}
}

// INI SECTION
IniParser::Section::Section(const String &name) :
_name(name),
_properties()
{
	int i = 0;
	int j = 100;
	i = j;
}

IniParser::Section::~Section()
{

}

const String &IniParser::Section::getString(const String &name, const String &default_value)
{
	if (!_properties.hasKey(name))
	{
		_properties[name] = default_value;
	}
	return _properties[name];
}

int IniParser::Section::getInteger(const String &name, const int &default_value)
{
	if ((!_properties.hasKey(name)) || (!_properties[name].isDigits()))
	{
		_properties[name] = String::Val(default_value);
	}
	return String::Int(_properties[name]);
}

double IniParser::Section::getDouble(const String &name, const float64 &default_value)
{
	if ((!_properties.hasKey(name)) || (!_properties[name].isDigits()))
	{
		_properties[name] = String::Val(default_value);
	}
	return String::Double(_properties[name]);
}

void IniParser::Section::putString(const String &name, const String &value)
{
	_properties[name] = value;
}

void IniParser::Section::putInteger(const String &name, const int &value)
{
	putString(name, String::Val(value));
}

void IniParser::Section::putDouble(const String &name, const float64 &value)
{
	putString(name, String::Val(value));
}
