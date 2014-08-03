#ifndef _SRL_URI_
#define _SRL_URI_

#include "srl/String.h"
#include "srl/util/Dictionary.h"

namespace SRL
{
    namespace Net
    {
        /**
        * URI
        * Handles the parsing of a Uniform Resource Identifier
        * 
        * scheme ":" [ "//" server ][:port] [ "/" ] [ path_segments ] [ "?" query ] [ "#" fragment ] 
        * 
        *   foo://example.com:8042/over/there/index.dtb;type=animal?name=ferret#nose
        *  \ /   \_________/ \__/\_________/ \___/ \_/ \_________/ \_________/ \__/
        *   |           |     |     |         |     |       |           |        |
        *scheme   authority  port path filename extension parameter(s) query   fragment
        *   |   ____________________|___
        *  / \ /                        \
        *  urn:example:animal:ferret:nose
        * 
        */
        class SRL_API URI
        {
        public:
            /** 
            * default contstructor 
            * @param uri        URI that this object will represetn
            * @param is_encoded set to true if the uri is already encoded
            */
            URI(const String& uri, bool is_encoded=false);
            /** default destructor */
            virtual ~URI();
            /** 
            * sets the URI that this object represents.
            * Useful when one wants to reuse the object instance
            * 
            * @param uri        URI that this object will represetn
            * @param is_encoded set to true if the uri is already encoded
            */
            void set(const String& uri, bool is_encoded=false);
            /** Assign another string to this */
            String& operator=(const String& s);
            
            /** returns the protocol */
            const String& scheme() const { return _scheme; }
            /** user[:password] if any */
            const String& user() const { return _user; }
            /** returns the host if any */
            const String& host() const { return _host; }
            /** returns the port if any, 0 for undefined */
            const uint32& port() const { return _port; }
            /** returns the path portion o the uri */
            const String& path() const { return _path; }
            /** returns the query string */
            const String& query() const { return _query; }
            /** returns the query string */
            const String& fragment() const { return _fragment; }
            /** add a key, value pair to our query list */
            void addQueryField(const String& key, const String& value);         
            /** returns a dictionary with the parsed query string */
            Util::Dictionary<String, String>& getQueryMap();

            /** returns the URI as an URI encoded String */
            const String& asString();
            
        // STATIC
            static const String ENCODE_EXCLUDE_USER;
            static const String ENCODE_EXCLUDE_HOST;
            static const String ENCODE_EXCLUDE_PATH;
            static const String ENCODE_EXCLUDE_QUERY;
            static const String ENCODE_EXCLUDE_QUERYITEM;
            static const String ENCODE_EXCLUDE_FRAG;

            /** 
            * Checks wether or not a character needs to be encoded
            * 
            * @param ch     character to check
            * @param exclude_list   list of characters that wowuld normally be 
            * encoded that we want to exclude for this character
            * @return   true if the character should be encoded
            */
            static bool ShouldEncode(const char& ch, const String& exclude_list);
            /**
            * Percent encodes a String
            * 
            * @param input      the string to be encoded
            * @param output     the newly encoded string
            * @param exclude    any characters that would normally be encoded that we want to exclude
            */
            static void Encode(const String& input, String& output, const String& exclude);

            /**
            * Decodes any percent encodings in a string
            * 
            * @param input      the string to be decoded
            * @param output     the newly decoded string
            */
            static void Decode(const String& input, String& output);            
            
        protected: // internal member functions
            /** the query string is not parsed until specifically told */
            void parseQueryString();
            void _parseScheme(const String& uri, int& pos);
            void _parseAuthority(const String& uri, int& pos, bool is_encoded);
            void _parsePath(const String& uri, int& pos, bool is_encoded);
            void _generateString();
        protected: // internal members
            // map of MIME-type headers
            Util::Dictionary<String, String> _query_map;
            // the parts
            String _str;
            String _scheme, _user, _host, _path, _query, _fragment;
            uint32 _port;
            
        };
    } /* Net */
    
} /* SRL */


#endif /* _SRL_URI_ */
