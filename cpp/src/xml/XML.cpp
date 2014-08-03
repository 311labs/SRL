#include "srl/xml/XML.h"
#include <string.h>

using namespace SRL;

                /** copy constructor */
XML::Parser::Attribute::Attribute(const Attribute &attr) : 
_element(attr._element),
_name(attr._name),
_value(attr._value)
{

}

XML::Parser::Attribute& XML::Parser::Attribute::operator=(const Attribute& attr)
{
    this->_name = attr._name;
    this->_value = attr._value;
    return *this;
}


XML::Parser::Element::Element(XML::Parser &parser, bool null_element) :
_parser(parser),
_attribute(*this),
_name(64),
_attributes(128), 
_content(1024),
_is_closing(false), _has_attributes(false), _is_null(null_element),
_apos(0)
{
    
}

XML::Parser::Element::~Element()
{
    
}

void XML::Parser::Element::clear()
{
    _name.clear();
    _apos = 0;
    _attributes.clear();
    _content.clear();
}

const String& XML::Parser::Element::getContent() throw(InvalidXML)
{
    if (_content.length() > 0)
        return _content;
    
    _parser.parseContent();
    return _content;
}

void XML::Parser::Element::getContent(int& data) throw(InvalidXML)
{
    // parse the content out
    data = String::Int(getContent());
}

void XML::Parser::Element::getContent(long& data) throw(InvalidXML)
{
    // parse the content out
    data = String::Long(getContent());
}

void XML::Parser::Element::getContent(float& data) throw(InvalidXML)
{
    // parse the content out
    data = String::Float(getContent());
}

void XML::Parser::Element::getContent(double& data) throw(InvalidXML)
{
    // parse the content out
    data = String::Double(getContent());
}

XML::Parser::Attribute* XML::Parser::Element::findAttribute(const String& name)
{
    // are we starting from the beggining
    uint32 start_pos = _apos;
    XML::Parser::Attribute* attr = getNextAttribute();
    while (attr != NULL)
    {
        if (attr->name() == name)
            return attr;
        attr = getNextAttribute();
        if (start_pos > 0)
        {
            if  (attr == NULL)
            {
                _apos = 0;
                attr = getNextAttribute();
            }
            else if (start_pos == _apos)
            {
                return NULL;
            }
        }
    }
    return NULL;
}

XML::Parser::Attribute* XML::Parser::Element::getNextAttribute()
{
    if ((_has_attributes)&&(_apos < _attributes.length()))
    {
        // parse the attribute name
        int spos = _attributes.find('=', _apos);
        if (spos == -1)
            return NULL;
        _attribute._name = _attributes.substr(_apos, spos-_apos);
        //printf("attr name: '%s'\n", _attribute._name.text());
        _apos = spos+1;
        // now parsing the value is a little more tricky, lets check for quotes
        // TODO should ignore more whitespace
        while ((_apos < _attributes.length()) && isspace(_attributes[_apos]))
            ++_apos;
        
        if (_apos >= _attributes.length())
            throw InvalidXML(String::Format("failed to parse attribute value: '%s'", _attribute._name.text()));
        
        char end_char = ' ';
        if ((_attributes[_apos] == '\'') || ((_attributes[_apos] == '\"')))
        {
            end_char = _attributes[_apos];
            ++_apos;
        }

        // now lets walk through the data to find the value
        int epos = 0;
        for(uint32 i=_apos; i<_attributes.length(); ++i)
        {
            if (_attributes[i] == end_char)
            {
                epos = i;
                break;
            }
        }

        if (epos > 0)
        {
            _attribute._value = _attributes.substr(_apos, epos-_apos);
            _apos = epos+1;
            if ((end_char == '\'') || ((end_char == '\"')))
                ++_apos;
        }
        else
        {
            // at then end or we failed to parse the end quote
            if (end_char != ' ')
                throw InvalidXML(String::Format("failed to find end to attribute value: '%s", _attribute._name.text()));
            _attribute._value = _attributes.substr(_apos);
            _apos = _attributes.length();            
        }
        
        //printf("attr value: '%s'\n", _attribute._value.text());
        return &_attribute;
    }
    return NULL;
}

XML::Parser::Parser(FS::TextFile* xml_file) :
_element(*this), _null_element(*this, true),
_xml_file(xml_file),
_xml(1024),
_pos(0)
{
    
}

XML::Parser::Parser(const String& xml) :
_element(*this), _null_element(*this, true),
_xml_file(NULL),
_xml(xml),
_pos(0)
{
    
}

XML::Parser::~Parser()
{

}

bool XML::Parser::_getXML(bool reverse)
{
    _pos = 0;
    _xml.clear();    
    if (_xml_file != NULL)
    {
        if (!reverse)
        {
            if (_xml_file->eof() == false)
            {
                return _xml_file->read(_xml, 1024) > 0;
            }
        }
        else
        {
            if (_xml_file->atBeginning() == false)
            {
                uint32 pos = _xml_file->position();
                if (pos >= 2048)
                {
                    pos -= 2048;
                }
                else if (pos > 1024)
                {
                    pos = 0;
                }
                else
                {
                    return false;
                }

                _xml_file->seek(pos);
                return _xml_file->read(_xml, 1024) > 0;

            }
        }
    }
    return false;
}

XML::Parser::Element* XML::Parser::getNextElement() throw(InvalidXML)
{
    _element.clear();
    // are we at the end
    if (_pos >= _xml.length())
    {
        // get more xml data
        if (_getXML() == false)
        {
            return NULL;
        }
    }
    
    // find the next tag, we will skip any text or content if need be
    int pos = _xml.find('<', _pos);
    while (pos == -1)
    {
        // get more xml data
        if (_getXML() == false)
        {
            return NULL;
        }
        pos = _xml.find('<', _pos);
    }
    // move the pos pointer past the '<' character
    _pos = pos+1;
    _parseElement();
    return &_element;
}

void XML::Parser::_parseElement()
{
    // now lets find the end of the tag
    int epos = _xml.find('>', _pos);
    while (epos == -1)
    {
        _element._name += _xml.substr(_pos);
        // sanity check tag
        if (_element._name.length() > 124)
            throw InvalidXML(String::Format("tag name exceeds allowed length: '%s'", _element._name.text()));
        // get more xml data
        if (_getXML() == false)
            throw InvalidXML(String::Format("can not find end to tag: '%s'", _element._name.text()));
        epos = _xml.find('>', _pos);
    }
    _element._name += _xml.substr(_pos, epos-_pos);
    _pos = epos+1;
    
    // check if this is a closing tag
    if (_element._name[0] == '/')
    {
        _element._is_closing = true;
        uint32 rpos = 0;
        _element._name.remove(rpos); // removes the '/' character
    }
    else if (_element._name[_element._name.size()-1] == '/')
    {
        _element._is_closing = true;
        uint32 rpos = _element._name.size()-1;
        _element._name.truncate(rpos); // removes the '/' character
    }
    else if (_element._name[0] == '?')
    {
        uint32 rpos = 0;
        _element._name.remove(rpos);
        _element._name.truncate(_element._name.length()-1);
    }
    else
    {
        _element._is_closing = false;
    }
    
    // check if the tag has attributes
    if (_element._name.find('=') > 0)
    {
        _element._has_attributes = true;
        // find the first space and that is the beginning of the attributes
        int pos = _element._name.find(' ');
        if (pos == -1)
            throw InvalidXML(String::Format("failed to parse tag attributes: '%s'", _element._name.text()));
        _element._attributes = _element._name.substr(pos+1);
        _element._attributes.replace("\r\n", ' ');
        _element._attributes.replace('\n', ' ');
        _element._attributes.replace('\r', ' ');
        _element._name.truncate(pos);
        //printf("%s: has these attributes: '%s'", _element._name.text(), _element._attributes.text());
    }
    else
    {
        _element._has_attributes = false;
    }

}

XML::Parser::Element* XML::Parser::getPreviousElement() throw(InvalidXML)
{
    // find the current elements start position
    int pos = _pos;
    int count = 0;
    
    // skip the current element start and find the starting '<' for the previous
    while (count < 2)
    {
        pos = _xml.rfind('<', pos-1);
        if (pos < 0)
        {
            // go back on get old xml data if available
            if (_getXML(true) == false)
                return NULL;
            pos = _xml.length();
        }
        else
            ++count;
    }
    
    if (pos < 0)
        return NULL;
    
    
    _element.clear();
    _pos = pos+1;
    _parseElement();
    return &_element;
}

XML::Parser::Element* XML::Parser::findNextElement(const String& name, bool is_closing) throw(InvalidXML)
{
    XML::Parser::Element *el = getNextElement();
    while (el != NULL)
    {
        if ((el->name() == name) && (el->isClosing() == is_closing))
            return el;
        el = getNextElement();
    }
    return NULL;
}

XML::Parser::Element* XML::Parser::findPreviousElement(const String& name, bool is_closing) throw(InvalidXML)
{
    XML::Parser::Element *el = getPreviousElement();
    while (el != NULL)
    {
        if ((el->name() == name) && (el->isClosing() == is_closing))
            return el;
        el = getPreviousElement();
    }
    return NULL;
}


bool XML::Parser::hasContent() throw(InvalidXML)
{
    if (_element._content.length() > 0)
        return true;
        
    if (_element._is_closing)
        return false;
    
    // the horrible loop forever... we really assume that _getXML will not allow this
    while (_xml.length() > 0)
    {
        for(uint32 i=_pos; i<_xml.length(); ++i)
        {
            // ignore white space
            if (isspace(_xml[i]))
                continue;
                
            if (_xml[i] == '<')
                return false;
            
            return true;
        }
        
        // get more xml data
        if (_getXML() == false)
            throw InvalidXML(String::Format("could not detect content, no end tag: '%s'", _element._name.text()));
    }

    // never ever should get here
    return false;

}

void XML::Parser::parseContent()
{
    int epos = _xml.find('<', _pos);
    while (epos == -1)
    {
        _element._content += _xml.substr(_pos);
        if (_getXML() == false)
            throw InvalidXML(String::Format("could not parse content, no end tag: '%s'", _element._name.text()));
        epos = _xml.find('<', _pos);
    }
    _element._content += _xml.substr(_pos, epos-_pos);
}








// Returns contents between <tag> and </tag>, updates offset to char after </tag>
String XML::parseTag(const char* tag, const String& xml, int* offset)
{
  if (*offset >= int(xml.length()))
      return String();
  int istart = xml.find(tag, *offset);
  if (istart < 0)
      return String();
  istart += strlen(tag);
  String etag = "</";
  etag += tag + 1;
  int iend = xml.find(etag, istart);
  if (iend < 0) return String();

  *offset = int(iend + etag.length());
  return xml.substr(istart, iend-istart).toLower();
}


// Returns true if the tag is found and updates offset to the char after the tag
bool XML::findTag(const char* tag, const String& xml, int* offset)
{
  if (*offset >= int(xml.length()))
      return false;
  int istart = xml.find(tag, *offset);
  if (istart < 0)
    return false;

  *offset = int(istart + strlen(tag));
  return true;
}


// Returns true if the tag is found at the specified offset (modulo any whitespace)
// and updates offset to the char after the tag
bool XML::nextTagIs(const char* tag, const String& xml, int* offset)
{
  if (*offset >= int(xml.length()))
      return false;
  const char* cp = xml.c_str() + *offset;
  int nc = 0;
  while (*cp && isspace(*cp)) 
  {
    ++cp;
    ++nc;
  }

  int len = int(strlen(tag));
  if  (*cp && (strncmp(cp, tag, len) == 0)) 
  {
    *offset += nc + len;
    return true;
  }
  return false;
}

// Returns the next tag and updates offset to the char after the tag, or empty string
// if the next non-whitespace character is not '<'
String XML::getNextTag(const String& xml, int* offset)
{
  if (*offset >= int(xml.length())) 
      return String();

  int pos = *offset;
  const char* cp = xml.c_str() + pos;
  while (*cp && isspace(*cp)) {
    ++cp;
    ++pos;
  }

  if (*cp != '<') 
      return String();

  String s;
  do 
  {
    s += *cp;
    ++pos;
  } while (*cp++ != '>' && *cp != 0);

  *offset = int(pos);
  return s;
}







// xml encodings (xml-encoded entities are preceded with '&')
static const char  AMP = '&';
static const char  RAW_ENTITY[] = { '<',   '>',   '&',    '\'',    '\"',    0 };
static const char* XML_ENTITY[] = { "lt;", "gt;", "amp;", "apos;", "quot;", 0 };
static const int   XML_ENTITY_LEN[] = { 3,     3,     4,      5,       5 };


// Replace xml-encoded entities with the raw text equivalents.

String XML::decode(const String& encoded)
{
    // check if we need to decode
    int i = encoded.find(AMP);
    if (i < 0)
        return encoded;

    int esize = encoded.size();
    // create a buffer atleast as big as the encoded string
    String decoded(encoded.c_str(), i);
    decoded.reserve(esize);

    const char* ens = encoded.c_str();
    while (i != esize) 
    {
        if (encoded[i] == AMP && i+1 < esize) 
        {
            int e;
            for (e=0; XML_ENTITY[e] != 0; ++e)
            {
                // compare the encoded entity to our list of known entities
                if (strncmp(ens+i+1, XML_ENTITY[e], XML_ENTITY_LEN[e]) == 0)
                {
                    decoded += RAW_ENTITY[e];
                    i += XML_ENTITY_LEN[e]+1;
                    break;
                }
            }
            // if we did not find the entity then add the unknown character and move on
            if (XML_ENTITY[e] == 0)    // unrecognized sequence
                decoded += encoded[i++];
        
        } 
        else // not an encoded entity, moving on 
        {
            decoded += encoded[i++];
        }
    }

    return decoded;
}


// Replace raw text with xml-encoded entities.
// FOR NOW WE WILL REMOVE CONTROL CHARACTERS
String XML::encode(const String& raw)
{
    // create our encoded buffer atleast as big as the raw buffer
    String encoded;
    encoded.reserve(raw.length());
    
    // go through the raw xml 1 character at a time
    const char* craw = raw.c_str();
    for(uint32 i = 0; i < raw.length(); ++i)
    {
        // check the character agains out entity list
        uint32 e;
        for(e = 0; RAW_ENTITY[e] != 0; ++e)
        {
            if (craw[i] == RAW_ENTITY[e])
            {
                encoded += AMP;
                encoded += XML_ENTITY[e];
                break;
            }
        }
        
        if (RAW_ENTITY[e] == 0)
        {
            if ((craw[i] < 9)||((craw[i] < 32)&&(craw[i] > 13)))
            {
                // TODO somehow encode control chacaters but for now just ignore them
            }
            else
            {
                encoded += craw[i];
            }
        }
    }
    
    return encoded;
}