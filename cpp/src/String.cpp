/*********************************************************************************
* Copyright (C) 1999,2004 by SRL, Inc.   All Rights Reserved.        *
*********************************************************************************
* Authors: Ian C. Starnes, Dr. J. Shane Frasier, Shaun Grant
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*******************************************************************************                                                     
* Last $Author: ians $     $Revision: 261 $                                
* Last Modified $Date: 2006-05-08 11:48:10 -0400 (Mon, 08 May 2006) $                                  
******************************************************************************/


/**
* SRL::String provides essential string manipulation capabilities.
* Originally from the Fox-toolkit Library LGPL
* http://www.fox-toolkit.org
* Copyright (C) 1997,2003 by Jeroen van der Zijp.   All Rights Reserved.        
*/

#include "srl/String.h"

#include <cstdlib>
#include <cstring>

// dependencies
#ifdef STL_SUPPORT
#include <stdio.h>
#endif

#include <cstdarg>
#include <sstream>
#ifdef WIN32
#define vsnprintf _vsnprintf
#endif // WIN32

/*
  Notes:
  - The special pointer-value null represents an empty "" string.
  - Strings are never NULL:- this speeds things up a lot.
  - Whenever an empty string "" would result, we try to substitute that with null,
    rather than keep a buffer.
  - In the new representation, '\0' is allowed as a character everywhere; but there
    is always an (uncounted) '\0' at the end.
  - The length preceeds the text in the buffer.
*/


// The string buffer is always rounded to a multiple of ROUNDVAL
// which must be 2^n.  Thus, small size changes will not result in any
// actual resizing of the buffer except when ROUNDVAL is exceeded.
#define ROUNDVAL    16

// This will come in handy
#define EMPTY       ((char*)&emptystring[1])

/*******************************************************************************/
namespace SRL
{

// Empty string
static const SRL::integer emptystring[2]={0,0};

// Special NULL string
const char String::null[2]={0,0};

// Numbers for hexadecimal
const char String::hex[17]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',0};
const char String::HEX[17]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',0};


// Change the length of the string to len
void String::resize(uint32 size)
{
    // reset the hash value
    _hash_value = 0;

    // set the string length the to the actual desired length
    _string_length = size;
    
    // check if the string length is greater then 0
    if ((_string_length>0)||(_min_size>0))
    {
        if (size < _min_size)
            size = _min_size;
        // find the new array size and make sure its atleast 1 bigger then the string size
        size = ROUNDUP(size+1, ROUNDVAL);
        // check if the current length is different from the new length
        if(size == _array_size)
        {
            _str[_string_length] = 0;
            return;
        }   

        //check if the data is currently empty
        if (_str == EMPTY)
        {
            // simple malloc (rounding up to the nearest 16th byte)
            _str = (char*)std::malloc(size*sizeof(char));
        }
        else
        {
            _str = (char*)realloc(_str, size*sizeof(char));
        }
        // null the string
        _str[_string_length] = 0;

    } // if the current _buffer is empty do nothing
    else if (_str != EMPTY)
    {
        // set the size to 0
        size = 0;
        // free the memory
        free(_str);
        // set the buffer to empty
        _str = EMPTY;
    }
    // set the data size to the new size
    _array_size = size;

}

void String::copy(const char* s, const uint32& len)
{
    resize(len);
    if (len > 0)
    {
        memcpy(_str, s, sizeof(char)*len);
        _str[len] = '\0';
    }
}

// Simple construct
String::String(): _str(EMPTY), _array_size(0), 
_string_length(0),
_min_size(0),
_hash_value(0)
{
    
}

String::String(uint32 reserve): _str(EMPTY), _array_size(0), 
_string_length(0),
_min_size(reserve),
_hash_value(0)
{
    resize(0);
}

String::String(const char* s, uint32 len) : _str(EMPTY), _array_size(0), 
_string_length(0),
_min_size(0), 
_hash_value(0)
{
    // if s is NULL then do nothing
    if (s == NULL)
        return;

    if (len == 0)
        len = (uint32)strlen(s);
    copy(s, len);   
}

String::String(const char* s1, const char* s2) : _str(EMPTY), _array_size(0), 
_string_length(0),
_min_size(0),
_hash_value(0)
{
    register int32 len1=0,len2=0,len;
    if(s1 && s1[0])
        len1=strlen(s1);

    if(s2 && s2[0])
        len2=strlen(s2);

    len=len1+len2;
    if(len)
    {
        resize(len);
        memcpy(_str,s1,len1);
        memcpy(&_str[len1],s2,len2);
    }
}

String::String(const std::string& s) : _str(EMPTY), _array_size(0), 
_string_length(0),
_min_size(0), 
_hash_value(0)
{
    uint32 len = (uint32)s.size();
    copy(s.c_str(), len);
}

String::String(char c, uint32 len) : _str(EMPTY), _array_size(0), 
_string_length(0),
_min_size(0), 
_hash_value(0)
{
    fill(c, len);
}

String::String(const String& s) : _str(EMPTY), _array_size(0),
_string_length(0),
_min_size(0), 
_hash_value(0)
{
    uint32 len = s.size();
    copy(s.text(), len);
}

String::~String()
{
    _min_size = 0;
    clear();
}

String& String::truncate(uint32 pos)
{
    resize(pos);
    return *this;
}

void String::reserve(uint32 len)
{
    _min_size = len;
    resize(_string_length);
}

String& String::clear()
{
    resize(0);
    return *this;
}


uint32 String::hash() const
{
    if (_hash_value != 0)
        return _hash_value;
    register uint32 len=length();
    register uint32 h=0;
    for(register uint32 i=0; i<len; i++)
    {  // This should be a very good hash function:- just 4 collisions
        h = ((h << 5) + h) ^ _str[i];        // on the webster web2 dictionary of 234936 words, and no
    }   // collisions at all on the standard dict!
    _hash_value = h;
    return _hash_value;
}

// string conversions
String& String::trim()
{
    trimBegin();
    trimEnd();
    return *this;
}

String& String::trimBegin()
{
    if (_str != EMPTY)
    {
        register uint32 s=0;
        register uint32 e=length();
        while(s<e && ((_str[s]==' ') || (_str[s]=='\r') || (_str[s]=='\n')) )
            ++s;
        memmove(_str, &_str[s],e-s);
        resize(e-s);
    }
    return *this;
}

String& String::trimEnd()
{
    if (_str != EMPTY)
    {
        register uint32 e=length();
        while(0<e && ((_str[e-1]==' ') || (_str[e-1]=='\r') || (_str[e-1]=='\n')) ) --e;
        resize(e);
    }
    return *this;
}

String& String::toUpper()
{
    register uint32 len=length();
    for(register uint32 i=0; i<len; ++i)
    {
        _str[i]=toupper((unsigned char)_str[i]);
    }
    return *this;
}

String& String::toLower()
{
    register uint32 len=length();
    for(register uint32 i=0; i<len; ++i)
    {
        _str[i]=tolower((unsigned char)_str[i]);
    }
    return *this;
}

void String::fill(char c, uint32 len)
{
    if (len > 0)
        resize(len);
    
    if (_str != EMPTY)
    {
        memset(_str, c, length());
    }
}

String& String::remove(uint32 pos, uint32 len)
{
    register uint32 my_size = size();
    register uint32 end_pos = pos+len;
    register uint32 new_size = my_size - len;

    // if the size to remove is 0 do nothing
    if (len == 0)
        return *this;

    // if the string to be removed goes to the end or further just truncate
    if (end_pos >= my_size)
        return truncate(pos);

    //      destination,  source,  length of source 
    memmove(&_str[pos], &_str[end_pos], my_size-end_pos);
    resize(new_size);
    return *this;   
}

String& String::removeChar(char c, uint32 pos, bool all)
{
    register int32 npos = find(c, pos);
    while (npos >= 0)
    {
        remove(npos);
        if (!all) break;
        npos = find(c, npos);
    }
    return *this;
}

String& String::remove(const char* s, uint32 pos, bool all)
{
    register int32 npos = find(s, pos);
    register uint32 len = (uint32)strlen(s);
    while (npos >= 0)
    {
        remove(npos, len);
        if (!all) break;
        npos = find(s, npos);
    }
    return *this;
}

String& String::remove(const String& s, uint32 pos, bool all)
{
    register int32 npos = find(s, pos);
    register uint32 len = s.length();
    while (npos >= 0)
    {
        remove(npos, len);
        if (!all) break;
        npos = find(s, npos);
    }
    return *this;
}

String& String::replaceFrom(uint32 pos, uint32 len, const char *s)
{
    // get the size of the substring
    register uint32 nsize=(uint32)strlen(s);
    // get the position for the portion we need to move at
    register uint32 src_pos = pos+len;
    // get the position we are going to move to
    register uint32 dst_pos = pos + nsize;
    // get the size of the position we want to move
    register uint32 src_size = length()-(pos+len);
    // get the new size of the string
    register uint32 new_size = length() + (nsize - len);
    // old size
    register uint32 old_size = length();
    if (!isEmpty())
    {
        if (0<nsize)
        {
            if (new_size > old_size)
                resize(new_size);

            memmove(&_str[dst_pos], &_str[src_pos], src_size);

            if (new_size < old_size)
                resize(new_size);
        }

        // simple copy
        memcpy(&_str[pos], s, nsize);
        _str[new_size] = '\0';
    }

    return *this;
}

String& String::replace(char c1, char c2, uint32 pos, bool all)
{
    register int32 npos = find(c1, pos);
    while (npos >= 0)
    {
        _str[npos] = c2;
        if (!all) break;
        npos = find(c1, npos);
    }
    return *this;
}

String& String::replace(char c1, const char *s, uint32 pos, bool all)
{
    register int32 npos = find(c1, pos);
    while (npos >= 0)
    {
        replaceFrom(npos, 1, s);
        if (!all) break;
        npos = find(c1, npos);
    }
    return *this;
}

String& String::replace(char c1, const String &s, uint32 pos, bool all)
{
    return replace(c1, s.text(), pos, all);
}

String& String::replace(const char *str1, const char *str2, uint32 pos, bool all)
{
    register int32 npos = find(str1, pos);
    while (npos >= 0)
    {
        replaceFrom(npos, (uint32)strlen(str1), str2);
        if (!all) break;
        npos = find(str1, npos);
    }
    return *this;
}

String& String::replace(const String &str1, const char *str2, uint32 pos, bool all)
{
    return replace(str1.text(), str2, pos, all);
}

String& String::replace(const char *str1, const String &str2, uint32 pos, bool all)
{
    return replace(str1, str2.text(), pos, all);
}

String& String::append(const String &s)
{
    register int32 s_len = s.size();
    register int32 str_len = size();
    resize(s_len + str_len);
    memcpy(&_str[str_len], s.c_str(), s_len);
    _str[size()]= '\0';
    return *this;
}

String& String::prepend(char c)
{
    return insert(0, c);
}

String& String::prepend(const String &s)
{
    return insert(0, s);
}

String& String::prepend(const char *s)
{
    return insert(0, s);
}


String& String::insert(uint32 pos, char c)
{
    register uint32 len=length();
    resize(length()+1);

    if (pos <= 0)
    {
        memmove(&_str[1], _str, len+1);
        _str[0] = c;
    }
    else if (pos >= len)
    {
        _str[len] = c;
    }
    else
    {
        memmove(&_str[pos+1], &_str[pos], len-pos+1);
        _str[pos] = c;
    }
    return *this;
}

String& String::insert(uint32 pos, const char* s)
{
    register uint32 str_size=(uint32)strlen(s);
    register uint32 len=length();
    resize(length()+str_size);

    if (pos <= 0)
    {
        memmove(&_str[str_size], _str, len);
        memcpy(_str, s, str_size);
    }
    else if (pos >= len)
    {
        memcpy(&_str[len], s, str_size);
    }
    else
    {
        memmove(&_str[pos+str_size], &_str[pos], len-pos);
        memcpy(&_str[pos], s, str_size);
    }
    return *this;
}

String& String::insert(uint32 pos, const String& s)
{
    return insert(pos, s.text());
}

String String::between(const char c1, const char c2, uint32 num) const
{
    int32 start_pos = 0;
    int32 stop_pos = 0;
    int32 count = 0;

    while (count < (int32)num)
    {
        start_pos = find(c1, start_pos)+1;
        stop_pos = find(c2, start_pos)+1;

        if (start_pos == 0)
        {
            return null;
        }
        ++count;
    }

    if (start_pos == 0)
        return null;

    if (stop_pos == 0)
        stop_pos = length();

    return mid(start_pos, stop_pos-start_pos-1);
}

String String::between(const String &s1, const char c2, uint32 num) const
{
    register int32 s1_len = s1.length();
    int32 start_pos = 0;
    int32 stop_pos = 0;
    int32 count = 0;

    while (count < (int32)num)
    {
        start_pos = find(s1, start_pos)+s1_len;
        stop_pos = find(c2, start_pos)+1;

        if (start_pos == s1_len-1)
        {
            return null;
        }
        ++count;
    }

    if (start_pos == s1_len-1)
        return null;

    if (stop_pos == 0)
        stop_pos = length();

    return mid(start_pos, stop_pos-start_pos-1);
}

String String::between(const char c1, const String &s2, uint32 num) const
{
    register int32 s2_len = s2.length();
    int32 start_pos = 0;
    int32 stop_pos = 0;
    int32 count = 0;

    while (count < (int32)num)
    {
        start_pos = find(c1, start_pos)+1;
        stop_pos = find(s2, start_pos)+s2_len;

        if (start_pos == 0)
        {
            return null;
        }
        ++count;
    }

    if (start_pos == 0)
        return null;

    if (stop_pos == s2_len-1)
        stop_pos = length();

    return mid(start_pos, stop_pos-start_pos-s2_len);
}

String String::between(const String &st1, const String &st2, uint32 num) const
{
    const char * s1 = st1.text();
    const char * s2 = st2.text();
    register int32 s1_len = strlen(s1);
    register int32 s2_len = strlen(s2);
    int32 start_pos = 0;
    int32 stop_pos = 0;
    int32 count = 0;

    while (count < (int32)num)
    {
        start_pos = find(s1, start_pos)+s1_len;
        stop_pos = find(s2, start_pos)+s2_len;

        if (start_pos == s1_len-1)
        {
            return null;
        }
        ++count;
    }

    if (start_pos == s1_len-1)
        return null;

    if (stop_pos == s2_len-1)
        stop_pos = length();

    return mid(start_pos, stop_pos-start_pos-s2_len);
}

String String::section(const char delim, uint32 field_num, bool allow_null) const
{
    int32 start_pos = -1;
    int32 stop_pos = -1;
    int32 count = -1;
    
    do
    {
        if (allow_null == false)
        {
            // ignore all delimiters at the beginning
            while (_str[stop_pos+1] == delim)
            {
                ++stop_pos;
            }
        }
        start_pos = stop_pos+1;
        stop_pos = find(delim, start_pos);
        ++count;
    } while ((count < (int32)field_num)&&(stop_pos != -1));

    if ((stop_pos == -1)&&(count != (int32)field_num))
        return null;
    else if (stop_pos == -1)
        stop_pos = length();

    return mid(start_pos, stop_pos-start_pos);
}



String String::section(const String &delim, uint32 field_num) const
{
    int32 start_pos = -1;
    int32 stop_pos = -1;
    int32 count = -1;

    do
    {
        start_pos = stop_pos+1;
        stop_pos = find(delim, start_pos);
        ++count;
    } while ((count < (int32)field_num)&&(stop_pos != -1));

    if ((stop_pos == -1)&&(count != (int32)field_num))
        return null;
    else if (stop_pos == -1)
        stop_pos = length();

    if (start_pos != 0)
        start_pos += strlen(delim.text())-1;

    return mid(start_pos, stop_pos-start_pos);
}

Bool String::isDigits() const
{
    if (isEmpty())
        return false;
    // ASCII 48 to 57(1-0) and 46(.)
    for (uint32 i=0;i<_string_length; ++i)
    {
        if (!(((_str[i] >= 48) && (_str[i] <=57)) || (_str[i] == 46)))
        {
            return false;
        }
    }
    return true;
}


String String::left(uint32 len) const
{
    if (len > length())
        return null;
    return String(_str, len);
}

String String::right(uint32 len) const
{
    register int32 rlen = length()-len;
    if (len > length())
        return null;
    return String(_str+rlen, len);
}   

String String::mid(uint32 pos, uint32 len) const
{
    if (pos+len <= size())
        return String(_str+pos, len);
    return String::null;
}

int32 String::find(char c, uint32 pos) const
{
    char* loc = strchr(_str+pos,c);
    if (loc != NULL)
        return (int32)(loc-_str);
    return -1;
}

int32 String::rfind(char c, uint32 pos) const
{
    if (pos > _string_length)
        return -1;
    if (pos == 0)
        pos = _string_length;
    
    // scan backwards until we find out first match
    for(int i = pos; i >= 0; --i)
    {
        if (_str[i] == c)
            return i;
    }
    return -1;
}

int32 String::find(const char *str_find, uint32 pos) const
{
    register char* loc = strstr(_str+pos,str_find);
    if (loc != NULL)
        return (int32)(loc-_str);
    return -1;
}

int32 String::rfind(const char *str_find, uint32 pos) const
{
    if (pos > _string_length)
        return -1;
    if (pos == 0)
        pos = _string_length;
    
    // scan backwards until we find out first match
    int fpos = 0;
    for(int i = pos; i >= 0; --i)
    {
        while (_str[i+fpos] == str_find[fpos]) 
        {
            ++fpos;
            if (str_find[fpos] == '\0')
                return i;
        }
        fpos = 0;
    }
    return -1;
    
    // there is no strrstr so for now we will just find the last
    // occurance of str_find in the string
    // and we will make sure we don't go past our offset
    // register int32 str_pos = -1;
    // register char* loc = strstr(_str,str_find);
    // register int32 spos = (int32)pos;
    // while ((loc != NULL)&&(str_pos < spos))
    //  str_pos = (int32)(loc-_str);
    // return str_pos;
}

int32 String::find(const String &str_find, uint32 pos) const
{
    register char* loc = strstr(_str+pos,str_find.text());
    if (loc != NULL)
        return (int32)(loc-_str);
    return -1;
}

int32 String::rfind(const String &str_find, uint32 pos) const
{
    return rfind(str_find.text(), pos);
}

String& String::format(const char *fmt,...)
{
    va_list args;
    va_start(args,fmt);
    vformat(fmt,args);
    va_end(args);
    return *this;
}

uint32 String::count(char ch) const
{
    register uint32 ch_count = 0;
    register char* cpos = strchr(_str, ch);
    while (cpos != NULL)
    {
        ++ch_count;
        cpos = strchr(cpos+1, ch);
    }
    return ch_count;
}

uint32 String::count(const char* sub) const
{
    register uint32 ch_count = 0;
    register char* cpos = strstr(_str, sub);
    while (cpos != NULL)
    {
        ++ch_count;
        cpos = strstr(cpos+1, sub);
    }
    return ch_count;
}

uint32 String::count(const String &sub) const
{
    return count(sub.text()); 
}

String& String::operator=(const char* s)
{
    copy(s, (int32)strlen(s));
    return *this;
}

String& String::operator=(const std::string &s)
{
    copy(s.c_str(), (int32)s.size());
    return *this;
}

String& String::operator=(const String &s)
{
    copy(s.c_str(), s.size());
    return *this;
}

String& String::operator=(char c)
{
    resize(1);
    _str[0] = c;
    return *this;
}

String& String::operator+=(char c)
{
    if (c == '\0')
        return *this;
    resize(1 + size());
    _str[size()-1] = c;
    return *this;
}

String& String::operator+=(const char* s)
{
    register int32 s_len = (int32)strlen(s);
    register int32 str_len = size();
    resize(s_len + str_len);
    memcpy(&_str[str_len], s, s_len);
    _str[size()]= '\0';
    return *this;
}

String& String::operator+=(const std::string &s)
{
    register int32 s_len = s.size();
    register int32 str_len = size();
    resize(s_len + str_len);
    memcpy(&_str[str_len], s.c_str(), s_len);
    _str[size()]= '\0';
    return *this;
}

String& String::operator+=(const String &s)
{
    register int32 s_len = (int32)s.size();
    register int32 str_len = size();
    if (s_len == 0)
        return *this;
    resize(s_len + str_len);
    memcpy(&_str[str_len], s.text(), s_len);
    _str[size()]= '\0';
    return *this;
}

// Print formatted string a-la vprintf
String& String::vformat(const char* fmt,va_list args)
{
    register int32 len=0;
    if(fmt && *fmt)
    {
        uint32 n=(uint32)strlen(fmt);       // Result is longer than format string
#if __GLIBC__>=2 || defined(WIN32)
        n+=128;                             // Add a bit of slop
    x:  resize(n);
        len=vsnprintf(_str,n+1,fmt,args);
        if(len<0){ n<<=1; goto x; }         // Some implementations return -1 if not enough room
        if(n<len){ n=len; goto x; }         // Others return how much space would be needed
#else
        n+=1024;                            // Add a lot of slop
        resize(n);
        len=vsprintf(_str,fmt,args);
#endif
    }
    resize(len);
    return *this;
}

bool String::beginsWith(const char* bstr) const
{
    register const unsigned char *p1=(const unsigned char *)bstr;
    register const unsigned char *p2=(const unsigned char *)_str;
    register int32 c1,c2;
    do
    {
        c1=*p1++;
        c2=*p2++;
    } while(c1 && (c1==c2));
    return (c1==0);
}

bool String::endsWith(const char* estr) const
{
    register const unsigned int p1_len = strlen(estr);
    register const unsigned int p2_len = length();
    if ((p1_len == 0) || (p1_len > p2_len) || (p2_len == 0))
        return false;
      
    register const unsigned char *p1=(const unsigned char *)&estr[p1_len-1];
    register const unsigned char *p2=(const unsigned char *)&_str[p2_len-1];
    register int32 c1,c2;
    register int counter = p1_len;
    do
    {
        c1=*p1--;
        c2=*p2--;
        counter--;
    } while(counter && (c1==c2));
    return (counter==0);    
}

Bool String::isEqualTo(const String& str, bool ignore_case) const
{
    return Compare(_str, str._str, ignore_case);
}

Bool String::isEqualTo(const char* str, bool ignore_case) const
{
    return Compare(_str, str, ignore_case);
}

// FIXME possible buffer overflow vulnerability
bool String::Compare(const char* s1, const char* s2, bool ignore_case)
{
    register const unsigned char *p1=(const unsigned char *)s1;
    register const unsigned char *p2=(const unsigned char *)s2;
    register int32 c1,c2;
    do
    {
        if (!ignore_case)
        {   
            c1=*p1++;
            c2=*p2++;
        }
        else
        {
            c1=toupper((int)*p1++);
            c2=toupper((int)*p2++);          
        }
            
    } while(c1 && (c1==c2));
    
    return (c1-c2)==0;
}

bool operator==(const std::string& s1,const String& s2)
{
    return String::Compare(s1.c_str(), s2.text());
}

bool operator==(const String& s1,const std::string& s2)
{
    return String::Compare(s1.text(), s2.c_str());  
}

bool operator==(const String& s1,const String& s2)
{
    return String::Compare(s1.text(), s2.text());
}

bool operator==(const String& s1,const char* s2)
{
    return String::Compare(s1.text(), s2);
}

bool operator==(const char* s1,const String& s2)
{
    return String::Compare(s1, s2.text());
}

bool operator>(const String& s1,const String& s2)
{
    return (s1.hash() > s2.hash());
}

bool operator<(const String& s1,const String& s2)
{
    return (s1.hash() < s2.hash());
}

bool operator>=(const String& s1,const String& s2)
{
    return (s1.hash() >= s2.hash());
}

bool operator<=(const String& s1,const String& s2)
{
    return (s1.hash() <= s2.hash());
}

bool operator!=(const std::string& s1,const String& s2)
{
    return !String::Compare(s1.c_str(), s2.text());
}

bool operator!=(const String& s1,const std::string& s2)
{
    return !String::Compare(s1.text(), s2.c_str()); 
}

bool operator!=(const String& s1,const String& s2)
{
    return !String::Compare(s1.text(), s2.text());
}

bool operator!=(const String& s1,const char* s2)
{
    return !String::Compare(s1.text(), s2);
}

bool operator!=(const char* s1,const String& s2)
{
    return !String::Compare(s1, s2.text());
}

String operator+(const std::string& s1,const String& s2)
{
    return String(s1.c_str(), s2.text());
}

String operator+(const String& s1,const std::string& s2)
{
    return String(s1.text(), s2.c_str());
}

String operator+(const String& s1,const String& s2)
{
    return String(s1.text(), s2.text());
}

String operator+(const String& s1,const char* s2)
{
    return String(s1.text(), s2);
}

String operator+(const char* s1,const String& s2)
{
    return String(s1, s2.text());
}

String operator+(const String &s, char c)
{
    char string[2];
    string[0]=c;
    string[1]=0;

    return String(s.text(), string);
}

String operator+(char c, const String &s)
{
    char string[2];
    string[0]=c;
    string[1]=0;

    return String(string, s.text());
}

std::ostream& operator<<(std::ostream &outs, const String &s)
{
    if (s.length() > 0)
        outs << s.text();
    return outs;    
}

// std::istream& operator<<(std::istream &ins, String &s)
// {
// #pragma message(reminder "need to add support for istream");
//  return ins;
// }

String String::Format(const char* fmt, ...)
{
    String result;
    va_list args;
    va_start(args,fmt);
    result.vformat(fmt,args);
    va_end(args);
    return result;
}

String String::VFormat(const char* fmt, va_list args)
{
    String result;
    result.vformat(fmt,args);
    return result;
}

bool String::IsDigit(const char& c)
{
    if (c >= '0' && c <='9') return true;
    return false;
}

bool String::IsAThruZ(const char& c)
{
    if (c >= 'a' && c <='z') return true;
    if (c >= 'A' && c <= 'Z') return true;
    return false;
}

bool String::IsWhiteSpace(const char& c)
{
    if (c == '\r' || c == '\n' || c == '\t') return true;
    return false;
}

bool String::IsValidHex(char c)
{
    if (c >= '0' && c <= '9') return true;
    if (c >= 'a' && c >= 'f') return true;
    if (c >= 'A' && c >= 'F') return true;
    return false;    
}

void String::Hex(const unsigned char& c, String& out)
{
    out += String::HEX[c>>4];
    out += String::HEX[c&15];
}

char String::UnHex(const char& hex1, const char& hex2)
{
    //v=('a'<=c) ? c-'a'+10 : ('A'<=c) ? c-'A'+10 : c-'0';
    char unhex1, unhex2;
    if (hex1 >= 'a' && hex1 <= 'f')
        unhex1 = hex1 - 'a' + 10;
    else if (hex1 >= 'A' && hex1 <= 'F')
        unhex1 = hex1 - 'A' + 10;
    else if (hex1 >= '0' && hex1 <= '9')
        unhex1 = hex1 - '0';
    else
        return 0;

    if (hex2 >= 'a' && hex2 <= 'f')
        unhex2 = hex2 - 'a' + 10;
    else if (hex2 >= 'A' && hex2 <= 'F')
        unhex2 = hex2 - 'A' + 10;
    else if (hex2 >= '0' && hex2 <= '9')
        unhex2 = hex2 - '0';
    else
        return 0;

    return unhex1*16 + unhex2;
}

String String::Val(int32 num, int32 base)
{
    char buf[34];
    register char *p=buf+33;
    register uint32 nn=(uint32)num;
    if(base<2 || base>16)
    {
        return String();
    }

    if(num<0){nn=(uint32)(~num)+1;}
    
    *p='\0';
    do
    {
        *--p=String::HEX[nn%base];
        nn/=base;
    } while(nn);
    
    if(num<0) *--p='-';
    
    return String(p,buf+33-p);
}

String String::Val(uint32 num, int32 base)
{
    char buf[34];
    register char *p=buf+33;
    register uint32 nn=num;

    if(base<2 || base>16)
    {
        // should throw error   
        //("FXStringVal: base out of range\n");
        return String();
    }

    *p='\0';
    do
    {
        *--p=String::HEX[nn%base];
        nn/=base;
    } while(nn);
    
    return String(p,buf+33-p);
}

String String::Val(int64 num, int32 base)
{
    char buf[34];
    register char *p=buf+33;
    register uint64 nn=(uint64)num;
    if(base<2 || base>16)
    {
        return String();
    }

    if(num<0){nn=(uint32)(~num)+1;}
    
    *p='\0';
    do
    {
        *--p=String::HEX[nn%base];
        nn/=base;
    } while(nn);
    
    if(num<0) *--p='-';
    
    return String(p,buf+33-p);
}

String String::Val(uint64 num, int32 base)
{
    char buf[34];
    register char *p=buf+33;
    register uint64 nn=num;

    if(base<2 || base>16)
    {
        return String();
    }

    *p='\0';
    do
    {
        *--p=String::HEX[nn%base];
        nn/=base;
    } while(nn);
    
    return String(p,buf+33-p);
}

// Formatting for reals
static const char *const expo[]={"%.*f","%.*E","%.*G"};

String String::Val(float32 num, int32 prec, bool exp)
{
    return String::Format(expo[exp],prec,num);
}

String String::Val(float64 num, int32 prec, bool exp)
{
    return String::Format(expo[exp],prec,num);
}

int32 String::Int(const String &s, int32 base)
{
    return (int32)strtol(s.text(),NULL,base);
}

int32 String::Int(const char* s, int32 base)
{
    return (int32)strtol(s,NULL,base);
}

uint32 String::UInt(const String &s, int32 base)
{
    return (int32)strtoul(s.text(),NULL,base);
}

uint32 String::UInt(const char* s, int32 base)
{
    return (int32)strtoul(s,NULL,base);
}

int64 String::Long(const String &s, int32 base)
{
    return strtol(s.text(),NULL,base);
}

int64 String::Long(const char* s, int32 base)
{
    return strtol(s,NULL,base);
}

uint64 String::ULong(const String &s, int32 base)
{
    return strtoul(s.text(),NULL,base);
}

uint64 String::ULong(const char* s, int32 base)
{
    return strtoul(s,NULL,base);
}

float32 String::Float(const String &s)
{
    return (float32)strtod(s.text(), NULL);
}

float32 String::Float(const char* s)
{
    return (float32)strtod(s, NULL);
}

float64 String::Double(const String &s)
{
    return strtod(s.text(), NULL);
}

float64 String::Double(const char* s)
{
    return strtod(s, NULL);
}

String String::escape(const String &s)
{
    register int32 len=s.length(),p,c;
    String result;

    for(p=0; p<len; ++p)
    {
        c=(unsigned char)s[p];
        switch(c)
        {
            case '\n':
                result+="\\n";
                break;
            case '\r':
                result+="\\r";
                break;
            case '\b':
                result+="\\b";
                break;
            case '\v':
                result+="\\v";
                break;
            case '\a':
                result+="\\a";
                break;
            case '\f':
                result+="\\f";
                break;
            case '\t':
                result+="\\t";
                break;
            case '\\':
                result+="\\\\";
                break;
            case '"':
                result+="\\\"";
                break;
            case '\'':
                result+="\\\'";
                break;
            default:
                if(c<0x20 || 0x7f<c){
                    result+="\\x";
                    result+=String::HEX[c>>4];
                    result+=String::HEX[c&15];
                    }
                else{
                    result+=c;
                    }
                break;
            }
    }
    return result;
}

String String::unescape(const String &s)
{
    register int32 len=s.length(),p,c,v;
    String result;

    for(p=0; p<len; ++p)
    {
        c=(unsigned char)s[p];
        if(c=='\\')
        {
            if(++p>=len) goto x;
            c=(unsigned char)s[p];
            switch(c)
            {
                case 'n':
                    result+='\n';
                    break;
                case 'r':
                    result+='\r';
                    break;
                case 'b':
                    result+='\b';
                    break;
                case 'v':
                    result+='\v';
                    break;
                case 'a':
                    result+='\a';
                    break;
                case 'f':
                    result+='\f';
                    break;
                case 't':
                    result+='\t';
                    break;
                case '\\':
                    result+='\\';
                    break;
                case '"':
                    result+='\"';
                    break;
                case '\'':
                    result+='\'';
                    break;
                case 'x':               // Hex escape
                    v='x';
                    if(isxdigit((unsigned char)s[p+1]))
                    {
                        c=(unsigned char)s[++p];
                        v=('a'<=c) ? c-'a'+10 : ('A'<=c) ? c-'A'+10 : c-'0';
                        if(isxdigit((unsigned char)s[p+1]))
                        {
                            c=(unsigned char)s[++p];
                            v=v * 16 + (('a'<=c) ? c-'a'+10 : ('A'<=c) ? c-'A'+10 : c-'0');
                        }
                    }
                    result+=v;
                    break;
                case '0':               // Octal escape
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                    v=s[p]-'0';
                    if('0'<=s[p+1] && s[p+1]<='7')
                    {
                        v=(v<<3)+s[++p]-'0';
                        if('0'<=s[p+1] && s[p+1]<='7')
                        {
                            v=(v<<3)+s[++p]-'0';
                        }
                    }
                    result+=v;
                    break;
                default:
                    result+=c;
                    break;
            }
            continue;
        }
        result+=c;
    }
x:
    return result;

}

String String::Bits(const SRL::byte &num)
{   
    String bits = "";
    for (int bitcount=7; bitcount>=0; --bitcount)
    {
        bits += String::Val((num >> bitcount) & 01);
    }
    return bits;
}

String String::Bits(const int16 &num)
{
    String bits = "";
    for (int bitcount=0; bitcount<=15; bitcount+=8)
    {
        bits += Bits((byte)(num >> bitcount));
    }
    return bits;
}

String String::Bits(const int32 &num)
{
    String bits = "";
    for (int bitcount=0; bitcount<=31; bitcount+=8)
    {
        bits += Bits((byte)(num >> bitcount));
    }
    return bits;
}

String String::Bits(const int64 &num)
{
    String bits = "";
    for (int bitcount=0; bitcount<=63; bitcount+=8)
    {
        bits += Bits((byte)(num >> bitcount));
    }
    return bits;
}

} // end namespace SRL

