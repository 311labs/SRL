#include "srl/net/URI.h"

using namespace SRL;
using namespace SRL::Net;

const String URI::ENCODE_EXCLUDE_USER("!$&'()*+,;=:");
const String URI::ENCODE_EXCLUDE_HOST("[]:");
const String URI::ENCODE_EXCLUDE_PATH("!$&'()*+,;=:@/");
const String URI::ENCODE_EXCLUDE_QUERY("!$&'*+,;=:@/?");
const String URI::ENCODE_EXCLUDE_QUERYITEM("!$'()*+,;:@/?");
const String URI::ENCODE_EXCLUDE_FRAG("!$&'()*+,;=:@/?");

URI::URI(const String& uri, bool is_encoded) :
_query_map(),
_str(124), _scheme(16), _user(16), 
_host(16), _path(64), _query(64), _fragment(16),
_port(0)
{
    set(uri, is_encoded);
}

URI::~URI()
{
    
}

// TODO change the _str to be mutable and the asString to check the is dirty flag
// this will help reduce the cost of adding query string fields

void URI::set(const String& uri, bool is_encoded)
{
    // scheme ":" [ "//" server ][:port] [ "/" ] [ path_segments ] [ "?" query ] [ "#" fragment ] 
    _query_map.clear();
    _str.clear(); _scheme.clear(); _user.clear(); _host.clear();
    _path.clear(); _query.clear(), _fragment.clear(); _port = 0;

    int pos = 0;
    _parseScheme(uri, pos);
    _parseAuthority(uri, pos, is_encoded);
    if (pos != -1)
        _parsePath(uri, pos, is_encoded);
    
    _generateString();
}

const String& URI::asString()
{
    if (_query_map.size() == 0)
        return _str;
        
    if (_query.count('&')+1 != _query_map.size())
    {
        // mismatch from query string fields and query map
        // assume new fields added so we will need to rebuild
        // query string
        _query.clear();
        Util::Dictionary<String, String>::Iterator iter=_query_map.begin();
        for (; iter!=_query_map.end(); iter++)
        {
            if (iter != _query_map.begin())
                _query += '&';
            _query += iter.key();
            _query += '=';
            
            // check for multiple values
            uint32 value_count = iter.value().count('&');
            if (value_count > 0)
            {
                _query += iter.value().section('&', 0);
                for(uint32 v=1; v<=value_count; ++v)
                {
                    _query += '&';
                    _query += iter.key();
                    _query += '=';
                    _query += iter.value().section('&', v);      
                }
            }
            else
            {
                _query += iter.value();
            }
        }
        
        _generateString();
    }
    
    return _str;
}

void URI::addQueryField(const String& key, const String& value)
{
    // this will parse the map if need be
    getQueryMap();
    // TODO encode query string key
    String encoded_value;
    URI::Encode(value, encoded_value, " .~_-");
    encoded_value.replace(' ', '+');
    if (_query_map.hasKey(key))
    {
        _query_map[key] += '&';
        _query_map[key] += encoded_value;
    }
    else
    {
        _query_map[key] = encoded_value;
    }
}

/** returns a dictionary with the parsed query string */
Util::Dictionary<String, String>& URI::getQueryMap()
{
    if ((_query_map.size() == 0) && (_query.length() > 0))
    {
        // parse the query string
        uint32 fields = _query.count('&')+1;
        int spos=0;
        int epos=0;
        String field(32);
        for(uint32 i=0; i<fields; ++i)
        {
            epos = _query.find('=', spos);
            if (epos == -1)
                break;
            field = _query.substr(spos, epos-spos);
            spos = epos+1;
            epos = _query.find('&', spos);
            if (epos == -1)
                epos = _query.length();
            // if the field already exists append the field delimited by a '&' since we
            // know if an & exists it is encoded
            if (_query_map.hasKey(field))
            {
                _query_map[field] += '&';
                _query_map[field] += _query.substr(spos, epos-spos);
             }
             else
             {
                 _query_map[field] = _query.substr(spos, epos-spos);
             }
             spos = epos+1;
        }
        
    }
    return _query_map;
}

void URI::_generateString()
{
    _str.clear();
    if (!_scheme.isEmpty())
    {
        _str = _scheme;
        _str += ":";
    }
    
    if (!_host.isEmpty())
    {
        _str += "//";
        if (!_user.isEmpty())
        {
            _str += _user;
            _str += '@';
        }
        
        _str += _host;
        if (_port > 0)
        {
            _str += ':';
            _str += String::Val(_port);
        }
    }
    
    if (!_path.isEmpty())
    {
        _str += _path;
        
        if (!_query.isEmpty())
        {
            _str += '?';
            _str += _query;
        }
        
        if (!_fragment.isEmpty())
        {
            _str += '#';
            _str += _fragment;
        }
    }
}

void URI::_parseScheme(const String& uri, int& pos)
{
    // SCHEME
    // looking for anything before the first ':' but not after the '/'
    int scheme_epos = uri.find(':');
    // if no ':' then there is no scheme
    if (scheme_epos == -1)
        return;
    
    int auth_spos = uri.find('/');
    
    // scheme
    if (scheme_epos < auth_spos)
    {
        _scheme = uri.left(scheme_epos);
        pos = scheme_epos + 1;
        return;
    }

    return;
}

void URI::_parseAuthority(const String& uri, int& pos, bool is_encoded)
{
    // Authority
    // looking for: // [user[:password]@] [server] [:] [port]
    // we store the authority into
    //      user:password into _authority
    //      server into _host
    //      port into _port
    int spos = uri.find("//");
    // if no ':' then there is no scheme
    if (spos == -1)
        return;
    spos += 2;
    // find the end of the authority part
    int epos = uri.find('/', spos);
    if (epos == -1)
        epos = uri.length();
        
    int part_epos = uri.find('@', pos);
    // check if we have a user authentication part
    if ((part_epos != -1) && (part_epos < epos))
    {
        if (is_encoded)
        {
            _user = uri.substr(spos, part_epos - spos);
        }
        else
        {
            _str = uri.substr(spos, part_epos - spos);
            URI::Encode(_str, _user, ENCODE_EXCLUDE_USER);
        }
        spos = part_epos+1;
    }
    
    // now parse the host portion
    part_epos = uri.find(':', spos);
    if ((part_epos == -1) || (part_epos > epos))
        part_epos = epos;
    else // we have a port to parse
    {
        ++part_epos;
        _port = String::Int(uri.substr(part_epos, epos-part_epos));
        --part_epos;
    }
    
    
    if (is_encoded)
    {
        _host = uri.substr(spos, part_epos - spos);
    }
    else
    {
        _str = uri.substr(spos, part_epos - spos);
        URI::Encode(_str, _host, ENCODE_EXCLUDE_HOST);
    }
    
    if (_port > 0)
        pos = epos+1;
        
    if (static_cast<uint32>(part_epos) < uri.length())
        pos = epos;
    else    
        pos = -1;
}

void URI::_parsePath(const String& uri, int& pos, bool is_encoded)
{
    // This actually parses the rest of the uri, path, query, fragments
    
    int query_pos = uri.find('?', pos);
    int frag_pos = uri.find('#', pos);    
    int epos = uri.length();
    int part_epos = epos;
    
    // QUERY STRING portion
    if (query_pos >= 0)
    {
        if (frag_pos > query_pos)
            epos = frag_pos;
        else
            epos = query_pos;
        
        ++query_pos;
        if (is_encoded)
        {
            _query = uri.substr(query_pos, part_epos - query_pos);
        }
        else
        {
            _str = uri.substr(query_pos, part_epos - query_pos);
            URI::Encode(_str, _query, " =&");
            _query.replace(' ', '+');
        }      
    }
    else if (frag_pos > 0)
    {
        epos = frag_pos;
    }
    
    // PATH portion
    if (is_encoded)
    {
        _path = uri.substr(pos, epos - pos);
    }
    else
    {
        _str = uri.substr(pos, epos - pos);
        URI::Encode(_str, _path, ENCODE_EXCLUDE_PATH);
    }    
    
    // FRAGMENT portion
    if (frag_pos >= 0)
    {
        if (query_pos > frag_pos)
            epos = query_pos;
        else
            epos = uri.length();
        
        ++frag_pos;
        if (is_encoded)
        {
            _fragment = uri.substr(frag_pos, epos - frag_pos);
        }
        else
        {
            _str = uri.substr(frag_pos, epos - frag_pos);
            URI::Encode(_str, _fragment, ENCODE_EXCLUDE_FRAG);
        }      
    }    
    pos = epos;
}

void URI::Encode(const String& input, String& output, const String& exclude)
{
    for(uint32 i=0; i<input.length(); ++i)
    {
        if (URI::ShouldEncode(input[i], exclude))
        {
            output += '%';
            // this call will append it to the string
            String::Hex(input[i], output);
        }
        else
            output += input[i];
    }
}

bool URI::ShouldEncode(const char& ch, const String& exclude_list)
{
    if (ch >= 'a' && ch <= 'z'
        || ch >= 'A' && ch <= 'Z'
        || ch >= '0' && ch <= '9'
        || ch == '-'
        || ch == '.'
        || ch == '_'
        || ch == '~'
        || exclude_list.find(ch) != -1)
        return false;
    return true;    
}

void URI::Decode(const String& encoded, String& output)
{
    // decode any '+' encodings
    String input = encoded;
    input.replace('+', ' ');
    for(uint32 i=0; i<input.length(); ++i)
    {
        if ((input[i] == '%') && (i+2 < input.length()))
        {
            // this call will append it to the string
            output += String::UnHex(input[i+1], input[i+2]);
            i += 2;
        }
        else
            output += input[i];
    }
}

// uint32 URI::GetSchemePort(const String& scheme)
// {
//     
// }