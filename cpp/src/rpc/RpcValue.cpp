#include "srl/rpc/RpcValue.h"
#include "srl/Exceptions.h"
#include "srl/util/Base64.h"
#include <stdio.h>

using namespace SRL;
using namespace SRL::Errors;
using namespace SRL::RPC;


static const char VALUE_TAG[]     = "value";
static const char BOOLEAN_TAG[]   = "boolean";
static const char DOUBLE_TAG[]    = "double";
static const char INT_TAG[]       = "int";
static const char I4_TAG[]        = "i4";
static const char STRING_TAG[]    = "string";
static const char DATETIME_TAG[]  = "dateTime.iso8601";
static const char BASE64_TAG[]    = "base64";

static const char ARRAY_TAG[]     = "array";
static const char DATA_TAG[]      = "data";

static const char STRUCT_TAG[]    = "struct";
static const char MEMBER_TAG[]    = "member";
static const char NAME_TAG[]      = "name";


const char* GetTag(const RpcValue::Type& value_type)
{
    switch(value_type)
    {
        case (RpcValue::TypeInt):
            return INT_TAG;
        case (RpcValue::TypeDouble):
            return DOUBLE_TAG;
        case (RpcValue::TypeBoolean):
            return BOOLEAN_TAG;
        case (RpcValue::TypeString):
            return STRING_TAG;
        case (RpcValue::TypeDateTime):
            return DATETIME_TAG;
        case (RpcValue::TypeArray):
            return ARRAY_TAG;
        case (RpcValue::TypeStruct):
            return STRUCT_TAG;
        case (RpcValue::TypeBase64):
            return BASE64_TAG;
        default:
            return NULL;
    }
    return NULL;
}


RpcValue* RpcValue::FromXml(XML::Parser& parser)
{
    // result to return
    RpcValue* rpc_value = NULL;
    // let get the current element
    XML::Parser::Element* element = parser.getCurrentElement();
    
    // now lets validate the current element is a value element
    if (element->name() != VALUE_TAG)
        return NULL;
    
    // check if the current tag has content, if it does assume it just a STRING VALUE
    if (element->hasContent())
    {
        rpc_value = new RpcValue(XML::decode(element->getContent()));
        return rpc_value;
    }
    
    // nowe this means we should have a specified value type
    if (parser.getNextElement() == NULL)
        throw IOException("invalid RPC XML format no tag");
    
    if (element->name() == STRING_TAG)
    {
        if (element->hasContent())
            rpc_value = new RpcValue(XML::decode(element->getContent()));
        else
            rpc_value = new RpcValue("");
        // get the close type element
        element = parser.getNextElement();
    }
    else if ((element->name() == INT_TAG) || (element->name() == I4_TAG))
    {
        // next lets get the text
        if (element->hasContent())
        {
            rpc_value = new RpcValue(element->getContent());
            rpc_value->_value.asInt = String::Int(element->getContent());
            rpc_value->_type = RpcValue::TypeInt;
        }        
        // get the close type element
        element = parser.getNextElement();
    }
    else if (element->name() == DOUBLE_TAG)
    {
        // next lets get the text
        if (element->hasContent())
        {
            rpc_value = new RpcValue(element->getContent());
            rpc_value->_value.asInt = String::Double(element->getContent());
            rpc_value->_type = RpcValue::TypeDouble;
        }        
        // get the close type element
        element = parser.getNextElement();
    }
    else if (element->name() == BOOLEAN_TAG)
    {
        // next lets get the text
        if (element->hasContent())
        {
            rpc_value = new RpcValue(element->getContent());
            rpc_value->_value.asInt = String::Int(element->getContent());
            rpc_value->_type = RpcValue::TypeBoolean;
        }        
        // get the close type element
        element = parser.getNextElement();
    }    
    else if (element->name() == DATETIME_TAG)
    {
        // next lets get the text
        if (element->hasContent())
        {
            rpc_value = new RpcValue(element->getContent());
            struct tm t;
            if (sscanf(rpc_value->_str.text(),"%4d%2d%2dT%2d:%2d:%2d",&t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec) != 6)
                return NULL;
                
            t.tm_year -= 1900;
            t.tm_isdst = -1;
            rpc_value->_value.asDateTime = new DateTime(&t);
            rpc_value->_type = RpcValue::TypeDateTime;
        }        
        // get the close type element
        element = parser.getNextElement();                
    }    
    else if (element->name() == ARRAY_TAG)
        rpc_value = new RpcArrayValue(parser);
    else if (element->name() == STRUCT_TAG)
        rpc_value = new RpcStructValue(parser);
    else if (element->name() == BASE64_TAG)
        rpc_value = new RpcBinaryValue(parser);

    // the next element should be the end value tag
    element = parser.getNextElement();
    return rpc_value; 
}

void RpcValue::clear()
{
    if (_type == TypeDateTime)
        delete _value.asDateTime;
    _type = TypeInvalid;
    _value.asDateTime = NULL;
    _str.clear();
    _xml.clear();
}

void RpcValue::set(const bool& value)
{
    clear();
    _type = TypeBoolean;
    _value.asBool = value;
    _str = value ? "1" : "0";
}

void RpcValue::set(const int& value)
{
    clear();
    _type = TypeInt;
    _value.asInt = value;
    _str = String::Val(value);
}

void RpcValue::set(const double& value)
{
    clear();
    _type = TypeDouble;
    _value.asInt = value;
    _str = String::Val(value);  
}

void RpcValue::set(const String& value)
{
    clear();
    _type = TypeString;
    _str = value;
}

void RpcValue::set(const char* value)
{
    clear();
    _type = TypeString;
    _str = value;
}

void RpcValue::set(const DateTime& value)
{
    clear();
    _type = TypeDateTime;
    _value.asDateTime = new DateTime(value);
    _str = value.formatString("%Y%m%dT%H:%M:%S");
}

const String& RpcValue::asString()
{
    return _str;
}

const String& RpcValue::asXml()
{
    _xml.reserve(124);
    _xml.clear();
    
    _xml += "<value><";
    _xml += GetTag(_type);
    _xml += ">";
    _xml += XML::encode(_str);
    _xml += "</";
    _xml += GetTag(_type);
    _xml += "></value>\n";
    return _xml;    
}

RpcStructValue::RpcStructValue(XML::Parser& parser)
{
    _type = TypeStruct;
    String mname;
    RpcValue* rpc_value;
    // every call to getNextElement() will change the element state
    XML::Parser::Element* element = parser.getCurrentElement();
    
    // parse through the struct assuming each tag is a member tag
    while ((parser.getNextElement() != NULL) && ((element->name() == MEMBER_TAG)&&(!element->isClosing())))
    {
        //printf("current element: '%s'\n", element->name().text());
        element = parser.getNextElement();
        // we are in the member element and its first child should be a name element
        if ((element == NULL) || (element->name() != NAME_TAG) || element->isClosing())
        {
            clear();
            throw IOException("struct has invalid xml, missing name");
        }
        
        // this tag should have text content
        if (!element->hasContent())
        {
            clear();
            throw IOException("struct name field has no data in xml");
        }
        // extract the name
        mname = element->getContent();  
        
        // now get the closing field for the name field
        if ((parser.getNextElement() == NULL) || (element->name() != NAME_TAG) || !element->isClosing())
        {
            clear();
            throw IOException("struct has invalid xml, missing name close");
        }       
        
        // now its time to go to the value elemen
        if ((parser.getNextElement() == NULL) || (element->name() != VALUE_TAG))
        {
            clear();
            throw IOException("struct has invalid xml, missing name");
        }
        // lets attempt to get an rpc value from the xml
        rpc_value = RpcValue::FromXml(parser);
        set(mname, rpc_value);
        // now go to the end value tag
        parser.getNextElement();
    }
}


RpcStructValue::~RpcStructValue()
{
    clear();
}

const String& RpcStructValue::asXml()
{
    _xml.reserve(256);
    _xml.clear();
    
    _xml += "<value>\n\t<struct>\n";
    
    Util::Dictionary<String, RpcValue*>::Iterator iter = _struct.begin();
    while (iter != _struct.end())
    {
        _xml += "\t\t<member>\n\t\t\t<name>";
        _xml += XML::encode(iter.key());
        _xml += "</name>\n\t\t\t";
        _xml += iter.value()->asXml();
        _xml += "\t\t</member>\n";
        ++iter;
    }
    _xml += "\t</struct>\n</value>\n";
    return _xml;
}

void RpcStructValue::clear()
{
    Util::Dictionary<String, RpcValue*>::Iterator iter = _struct.begin();
    while (iter != _struct.end())
    {
        delete iter.value();
        ++iter;
    }
    _struct.clear();    
    RpcValue::clear();
}

RpcValue* RpcStructValue::get(const String& name)
{
    if (_struct.hasKey(name))
        return _struct.get(name);
    return NULL;
}

// TODO: assert that the member rpcvalue is of a base type
void RpcStructValue::set(const String& name, const bool& value)
{
    if (hasMember(name))
    {
        _struct.get(name)->set(value);
    }
    else
    {
        _struct.add(name, new RpcValue(value));
    }
}

void RpcStructValue::set(const String& name, const uint32& value)
{
    int nval = static_cast<int>(value);
    set(name, nval);
}

void RpcStructValue::set(const String& name, const int& value)
{
    if (hasMember(name))
    {
        _struct.get(name)->set(value);
    }
    else
    {
        _struct.add(name, new RpcValue(value));
    }
}

void RpcStructValue::set(const String& name, const double& value)
{
    if (hasMember(name))
    {
        _struct.get(name)->set(value);
    }
    else
    {
        _struct.add(name, new RpcValue(value));
    }
}

void RpcStructValue::set(const String& name, const String& value)
{
    if (hasMember(name))
    {
        _struct.get(name)->set(value);
    }
    else
    {
        _struct.add(name, new RpcValue(value));
    }
}

void RpcStructValue::set(const String& name, const char* value)
{
    if (hasMember(name))
    {
        _struct.get(name)->set(value);
    }
    else
    {
        _struct.add(name, new RpcValue(value));
    }
}

void RpcStructValue::set(const String& name, const DateTime& value)
{
    if (hasMember(name))
    {
        _struct.get(name)->set(value);
    }
    else
    {
        _struct.add(name, new RpcValue(value));
    }
}


RpcArrayValue::RpcArrayValue(XML::Parser& parser) : RpcValue(), _array(32)
{
    _type = TypeArray;
    // every call to getNextElement() will change the element state
    XML::Parser::Element* element = parser.getCurrentElement();
    // we are in the member element and its first child should be a name element
    if ((parser.getNextElement() == NULL) || (element->name() != DATA_TAG))
    {
        clear();
        throw IOException("array has invalid xml, missing data tag");
    }

    while ((parser.getNextElement() != NULL) && (element->name() == VALUE_TAG) && (!element->isClosing()))
    {
        _array.add(RpcValue::FromXml(parser));
    }
    
    // the current tag should be a closing data tag    
    if ( (element->name() != DATA_TAG) || (!element->isClosing()) )
    {
        clear();
        throw IOException("array has invalid xml, missing closing data tag");
    }

    // now we want our closing array
    if ((parser.getNextElement() == NULL) || (element->name() != ARRAY_TAG) || (!element->isClosing()))
    {
        clear();
        throw IOException("array has invalid xml, missing closing array tag");
    }

}

RpcArrayValue::~RpcArrayValue()
{
    clear();
}

const String& RpcArrayValue::asXml()
{
    _xml.reserve(256);
    _xml.clear();
    
    _xml += "<value>\n\t<array>\n\t\t<data>\n";
    uint32 s = _array.size();
    for (uint32 i=0; i<s; ++i)
        _xml += _array.get(i)->asXml();
    _xml += "\t\t</data>\n\t\t</array>\n</value>\n";
    return _xml;
}

void RpcArrayValue::clear()
{
    // uint32 s = _array.size();
    // for (uint32 i=0; i<s; ++i)
    //     delete _array.get(i);
    _array.clear(true);
    RpcValue::clear();
}

// ==== RPC Base64 Value ====
RpcBinaryValue::RpcBinaryValue(XML::Parser& parser)
{
    // every call to getNextElement() will change the element state
    XML::Parser::Element* element = parser.getCurrentElement();

    if (!element->hasContent())
    {
        throw IOException("base64 tag has no content");
    }
    
    _type = TypeBase64;
    Base64::decode(element->getContent(), _bytes);
   
    // the next tag should be the end tag
    if ( (element->name() != BASE64_TAG) || (!element->isClosing()) )
        throw IOException("base64 tag has no end tag");

}


const String& RpcBinaryValue::asXml()
{
    _xml.reserve(256);
    _xml.clear();
    
    Base64::encode(_bytes.bytes(), _bytes.length(), _str);
    
    _xml += "<value><base64>";
    _xml += _str;
    _xml += "</base64></value>\n";

    return _xml;
}





