#include "srl/util/Base64.h"
#include "srl/io/ByteBuffer.h"

using namespace SRL;
// TODO clean this up to use BinaryReader and BinaryWriter

String Base64::encode(const String& raw)
{
    String encoded;
    Base64::encode(raw.bytes(), raw.length(), encoded);
    return encoded;
}

String Base64::encode(const SRL::byte* bytes, const uint32& len)
{
    String encoded;
    Base64::encode(bytes, len, encoded);
    return encoded;
}


void Base64::encode(const SRL::byte* bytes, const uint32& len, String& encoded)
{
    int encoded_size = len*4/3+4;
    encoded.reserve(encoded_size);

    for(uint32 i=0; i<len; i+=3) 
    {
        unsigned char b1=0, b2=0, b3=0, b4=0, b5=0, b6=0, b7=0;
        b1 = bytes[i];
        if(i+1<len)
            b2 = bytes[i+1];

        if(i+2<len)
            b3 = bytes[i+2];

        b4= b1>>2;
        b5= ((b1&0x3)<<4)|(b2>>4);
        b6= ((b2&0xf)<<2)|(b3>>6);
        b7= b3&0x3f;

        encoded += Base64::encode(b4);
        encoded += Base64::encode(b5);

        if(i+1<len) 
            encoded += encode(b6);
        else 
            encoded += '=';

        if(i+2<len) 
            encoded += encode(b7);
        else 
            encoded += '=';
    }  
}

String Base64::decode(const String& src)
{
    IO::ByteBuffer buf(src.length());
    Base64::decode(src, buf);
    buf.write((SRL::byte)0);
    return buf.c_str();
}

void Base64::decode(const String& src, IO::BinaryWriter& decoded)
{
    String encoded;
    encoded.reserve(src.length());

    uint32 src_len = src.length();
    /* Ignore non base64 chars as per the POSIX standard */
    for(uint32 k=0; k < src_len; ++k) 
    {
        if(Base64::isBase64(src[k])) 
        {
            encoded += src[k];
        }
    }
    
    // now lets get dirty
    const char* buf = src.c_str();
    for(uint32 i = 0; i < src_len; i+=4)
    {
        char c2='A', c3='A', c4='A';
        unsigned char b1=0, b2=0, b3=0, b4=0;
        char c1 = buf[i];
        
        if (i+1 < src_len)
            c2 = buf[i+1];
        
        if (i+2 < src_len)
            c3 = buf[i+2];
        
        if (i+3 < src_len)
            c4 = buf[i+3];
        
        b1= Base64::decode(c1);
        b2= Base64::decode(c2);
        b3= Base64::decode(c3);
        b4= Base64::decode(c4);
        
        decoded.write( (b1<<2)|(b2>>4) );
        if (c3 != '=')
            decoded.write( ((b2&0xf)<<4)|(b3>>2) );
        
        if (c4 != '=')
            decoded.write( ((b3&0x3)<<6)|b4 );
    }
}

/**
 * Base64 encode one byte
 */
char Base64::encode(const unsigned char& u) 
{

  if(u < 26)  return 'A'+u;
  if(u < 52)  return 'a'+(u-26);
  if(u < 62)  return '0'+(u-52);
  if(u == 62) return '+';
  
  return '/';
}


/**
 * Decode a base64 character
 */
unsigned char Base64::decode(const char& c) {
  
  if(c >= 'A' && c <= 'Z') return(c - 'A');
  if(c >= 'a' && c <= 'z') return(c - 'a' + 26);
  if(c >= '0' && c <= '9') return(c - '0' + 52);
  if(c == '+')             return 62;
  
  return 63;
}


/**
 * Return TRUE if 'c' is a valid base64 character, otherwise FALSE
 */
bool Base64::isBase64(const char& c) {

  if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
     (c >= '0' && c <= '9') || (c == '+')             ||
     (c == '/')             || (c == '=')) {
    
    return true;
    
  }
  return false;
}

