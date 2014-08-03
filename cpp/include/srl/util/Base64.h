#ifndef __SRLBASE64
#define __SRLBASE64

#include "srl/String.h"
#include "srl/io/BinaryWriter.h"

namespace SRL
{
    namespace Base64
    {
        /**
        * Base64 encode a string
        * 
        * @param raw    String that needs to be encoded
        * @returns String that is a Base64 encoded copy of the passed in string
        */
        SRL_API String encode(const String& raw);
        
        /**
        * Base64 encode an array of bytes
        * 
        * @param bytes    Bytes that needs to be encoded
        * @param len      length of the byte array to convert
        * @returns String Base64 encoded ASCII of the passed in bytes
        */
        SRL_API String encode(const byte* bytes, const uint32& len);       
        
        /**
        * Base64 encode an array of bytes and put the result in the passed in string
        * 
        * @param bytes    Bytes that needs to be encoded
        * @param len      length of the byte array to convert
        * @param result   String to put the encoded result into
        */
        SRL_API void encode(const byte* bytes, const uint32& len, String& result);        

        /**
        * Base64 encode single character
        * 
        * @param c    character to encode
        * @returns String that is a Base64 encoded copy of the passed in string
        */
        SRL_API char encode(const unsigned char& c);
        
        /**
        * Decode a Base64 string
        * 
        * @param raw    String that needs to be decoded
        * @returns String that is a Base64 decoded copy of the passed in string
        */
        SRL_API String decode(const String& encoded);

        /**
        * Base64 decode a string and put the result into the ByteBuffer
        * 
        * @param encoded  String that needs to be decoded
        * @param len      length of the byte array to convert
        * @param result   ByteBuffer to put the decoded result into
        */
        SRL_API void decode(const String& encoded, IO::BinaryWriter& result);

        /**
        * Decode a Base64 character
        * 
        * @param c    character to decode
        * @returns String that is a Base64 decoded copy of the passed in string
        */
        SRL_API unsigned char decode(const char& c);        
        
        /**
        * Check if the character is a valid base64 character
        * 
        * @param c    character to check
        * @returns True if the character is a valid base64 character
        */
        SRL_API bool isBase64(const char& c);        
        
        
    } /* Base64 */
    
} /* SRL */


#endif