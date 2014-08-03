/*********************************************************************************
* Copyright (C) 1999,2006 by Ian C. Starnes   All Rights Reserved.        
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
*******************************************************************************/

#ifndef __SRL_String__
#define __SRL_String__

#include "srl/types.h"
#include "srl/exports.h"

#ifdef STL_SUPPORT
    #include <string>
    #include <cstdarg>
#endif

namespace SRL
{
    class SRL_API String 
    {
    public:
        /** Create empty string */
        String();
        /** Create empty string that has a preallocated buffer of size reserved */
        String(uint32 reserve);
        /** Construct string from const char* ie: "hello" */
        String(const char* s, uint32 len=0);
#ifdef STL_SUPPORT
        /** Construct string from std::string */
        String(const std::string &std_str);
#endif
        /** Concatenate string from two c style strings */
        String(const char* s1, const char* s2);
        /** Create a string of specified length and fill it with character */
        String(char c, uint32 len);
        
        /** copy c string to string array */
        void copy(const char* s, const uint32& len);        
        
        /** destructor */
        ~String();

        /** Copy Constructor */
        String(const String &s);
        
        /** reserve len bytes for future text */
        void reserve(uint32 len);
        /** resize the string */
        void resize(uint32 len);

        /** return the size of the string */
        inline uint32 length() const{ return _string_length; }
        /** return a pointer to the c style string */
        const char* text() const { return (const char*)_str; }
        /** return a pointer to the raw c style string array
         * DANGER USE AT OWN RISK
         * this needs to change by allowing access another way
         */
        char *_strPtr() { return _str;}

        /** return the raw bytes */
        const byte* bytes() const { return (byte*)_str;}
        /** return the raw bytes */
        byte* _bytes() { return (byte*)_str;}

        /** check if string is empty */
        Bool isEmpty() const { return _string_length == 0; }
        /** check if the string is all digits / numbers */
        Bool isDigits() const;
        
        /** compare to strings ignoring the case */
        Bool isEqualTo(const String& str, bool ignore_case=true) const;
        /** compare to strings ignoring the case */
        Bool isEqualTo(const char* str, bool ignore_case=true) const;        
        
        /** Get hash value */
        uint32 hash() const;

    // string conversion
        /** remove leading and trailing whitespaces */
        String& trim();
        /** Remove leading whitespace */
        String& trimBegin();
        /** Remove trailing whitespace */
        String& trimEnd();

        /** convert entire string to uppercase */
        String& toUpper();
        /** convert entire string to lowercase */
        String& toLower();

        /** resize the string to the length and fill with character */
        void fill(char c, uint32 len=0);

        /** remove substring starting at pos of size len */
        String& remove(uint32 pos, uint32 len=1);
        /** remove all occurances of character starting at pos*/
        String& removeChar(char c, uint32 pos=0, bool all=true);
        /** remove all occurances of substrings */
        String& remove(const char* s, uint32 pos=0, bool all=true);
        /** remove all occurances of substrings */
        String& remove(const String& s, uint32 pos=0, bool all=true);

        /** replace substring starting at pos of size len */
        String& replaceFrom(uint32 pos, uint32 len, const char* s);

        /** replace all occurances of c1 with c2*/
        String& replace(char c1, char c2, uint32 pos=0, bool all=true);
        /** replace all occurances of c1 with c2*/
        String& replace(char c1, const char *str, uint32 pos=0, bool all=true);
        /** replace all occurances of c1 with str2*/
        String& replace(char c1, const String &str2, uint32 pos=0, bool all=true);
        /** replace all occurances of c1 with c2*/
        String& replace(const char *str1, const char *str2, uint32 pos=0, bool all=true);
        /** remove all occurances of substrings */
        String& replace(const char *str2, const String &str1, uint32 pos=0, bool all=true);
        /** remove all occurances of substrings */
        String& replace(const String &str1, const char *str2, uint32 pos=0, bool all=true);

        /** append string to the end */
        String& append(const String& s);
        /** Insert character at specified position */
        String& insert(uint32 pos, char c);
        /** Insert string at specified position */
        String& insert(uint32 pos, const String& s);
        /** Insert string at specified position */
        String& insert(uint32 pos, const char* s);

        /** Insert character at specified position */
        String& prepend(char c);
        /** Insert string at specified position */
        String& prepend(const String& s);
        /** Insert string at specified position */
        String& prepend(const char* s);

        /** Truncate string at pos */
        String& truncate(uint32 pos);
        /** clear the string */
        String& clear();

    // search for substrings 
        /** true if the string begins with the text */
        bool beginsWith(const char* begin_str) const;
        /** true if the string ends with the text */
        bool endsWith(const char* begin_str) const;
            
        /** return string between c1 and c2, at occurance number=num */
        String between(const char c1, const char c2, uint32 num=1) const;
        /** return string between s1 and s2 */
        //String between(const char *s1, const char *s2, uint32 num=1) const;
        /** return string between s1 and s2 */
        String between(const String& s1, const String& s2, uint32 num=1) const;
        /** return string between s1 and s2 */
        String between(const String& s1, const char c2, uint32 num=1) const;
        /** return string between s1 and s2 */
        String between(const char c2, const String& s1, uint32 num=1) const;
        /** return string between s1 and s2 */
        //String between(const char* s1, const String& s2, uint32 num=1) const;
        
        /** Return num partition(s) of string separated by delimiter delim */
        String section(const char delim, uint32 field_num=0, bool allow_null=true) const;
        /** Return num partition(s) of string separated by delimiters in delim */
        String section(const String& delim, uint32 field_num=0) const;
        /** Return num partition(s) of string separated by delimiters in delim */
        //String section(const char* delim, uint32 field_num=1) const;

        /** Get len characters from the left */
        String left(uint32 len) const;
        /** Get len characters from the right */
        String right(uint32 len) const;
        /** Get len characters from pos */
        String mid(uint32 pos, uint32 len) const;

        /** Find a character, searching forward; return position or -1 */
        int32 find(char c, uint32 pos=0) const;
        /** Find a character, searching backward; return position or -1 */
        int32 rfind(char c, uint32 pos=0) const;

        /** Find n-th occurrence of character, searching forward; return position or -1 */
        //int32 find(char c, uint32 pos, uint32 occur) const;
        /** Find n-th occurrence of character, searching backward; return position or -1 */
        //int32 rfind(char c, uint32 pos, uint32 occur) const;

        /** Find a substring, searching forward; return position or -1 */
        int32 find(const char* str_find, uint32 pos=0) const;
        /** Find a substring, searching backward; return position or -1 */
        int32 rfind(const char* str_find, uint32 pos=0) const;

        /** Find a substring, searching forward; return position or -1 */
        int32 find(const String& str_find, uint32 pos=0) const;
        /** Find a substring, searching backward; return position or -1 */
        int32 rfind(const String& str_find, uint32 pos=0) const;

        /** Return number of occurrences of ch in string */
        uint32 count(char ch) const;
        /** Return number of occurrences of string sub in string */
        uint32 count(const char* sub) const;
        /** Return number of occurrences of string sub in string */
        uint32 count(const String& sub) const;

    // array style access
        /** Return a non-const reference to the ith character */
        char& operator[](uint32 i){ return _str[i]; }
        /** Return a const reference to the ith character */
        const char& operator[](uint32 i) const { return _str[i]; }
    
    // assignment operator
        /** Assign a std string to this */
#ifdef STL_SUPPORT
        String& operator=(const std::string& s);
#endif
        /** Assign another string to this */
        String& operator=(const String& s);
        /** Assign a C-style string to this */
        String& operator=(const char* s);
        /** Assign a character to this */
        String& operator=(char c);
    /// Append operators
#ifdef STL_SUPPORT
        /** append std::string */
        String& operator+=(const std::string& s);
#endif
        /** append another string */
        String& operator+=(const String& s);
        /** append another c string */
        String& operator+=(const char* s);
        /** append character */
        String& operator+=(char c);

        /** return the size of the allocated array */
        uint32 allocatedSize() const { return _array_size; }
    // std::string interface
        /** return the size of the string (std::string style) */
        uint32 size() const{ return _string_length; }
        /** return a pointer to the c style string */
        const char* c_str() const { return (const char*)_str; }
        /** return string from pos of len size */
        String substr(uint32 pos, uint32 len) const { return mid(pos, len); }
        /** return entire string starting from pos */
        String substr(uint32 pos) const { return mid(pos, length()-pos); }

        /** Format a string a-la printf */
        String& format(const char* fmt,...) SRL_PRINTF(2,3) ;
        /** Format a string a-la printf */
        String& vformat(const char* fmt,va_list args);

    private:
        /** internal string array */
        char *_str;
        /** current size of string array */
        uint32 _array_size;
        /** current length of string */
        uint32 _string_length;
        /** array min length */
        uint32 _min_size;
        /** strings hash value */
        mutable uint32 _hash_value;


    public:
    // Comparison operators
        /** basic c style string comparison .. just as fast as strcmp if not faster */
        static bool Compare(const char* s1, const char* s2, bool ignore_case=false);
#ifdef STL_SUPPORT
        /** compare std string to our String */
        friend SRL_API bool operator==(const std::string& s1,const String& s2);
        /** compare our String to std string */
        friend SRL_API bool operator==(const String& s1,const std::string& s2);
#endif
        /** compare our String to our String */
        friend SRL_API bool operator==(const String& s1,const String& s2);
        /** compare our String to c style string */
        friend SRL_API bool operator==(const String& s1,const char* s2);
        /** compare c style string to our String */
        friend SRL_API bool operator==(const char* s1,const String& s2);

        /** compare c style string to our String */
        friend SRL_API bool operator>=(const char* s1,const String& s2);
        /** compare c style string to our String */
        friend SRL_API bool operator<=(const char* s1,const String& s2);
        /** compare c style string to our String */
        friend SRL_API bool operator<(const char* s1,const String& s2);
        /** compare c style string to our String */
        friend SRL_API bool operator>(const char* s1,const String& s2);                     

#ifdef STL_SUPPORT
        /** compare std string to our String */
        friend SRL_API bool operator!=(const std::string& s1,const String& s2);
        /** compare our String to std string */
        friend SRL_API bool operator!=(const String& s1,const std::string& s2);
#endif
        /** compare our String to our String */
        friend SRL_API bool operator!=(const String& s1,const String& s2);
        /** compare our String to c style string */
        friend SRL_API bool operator!=(const String& s1,const char* s2);
        /** compare c style string to our String */
        friend SRL_API bool operator!=(const char* s1,const String& s2);

    /// Concatenate two strings
#ifdef STL_SUPPORT
        /** concatenate an std string with our String */
        friend SRL_API String operator+(const std::string& s1,const String& s2);
        /** concatenate an our String with std string */
        friend SRL_API String operator+(const String& s1,const std::string& s2);
#endif
        /** concatenate an our String with out String */
        friend SRL_API String operator+(const String& s1,const String& s2);
        /** concatenate an our String with c style string */
        friend SRL_API String operator+(const String& s1,const char* s2);
        /** concatenate an c style string with our String */
        friend SRL_API String operator+(const char* s1,const String& s2);

    /// Concatenate with single character
        /* Concatenate with single character */
        friend SRL_API String operator+(const String& s,char c);
        /** Concatenate with single character */
        friend SRL_API String operator+(char c,const String& s);

#ifdef STL_SUPPORT
    /// Saving to a stream
        friend SRL_API std::ostream  & operator <<( std::ostream &outs, const String& s);
        //friend SRL_API std::istream & operator >>(std::istream  &ins, String& s);
#endif

    // public static class interface
        /// Format a string a-la printf
        static String Format(const char* fmt,...) SRL_PRINTF(1,2) ;
        static String VFormat(const char* fmt,va_list args);

        /**
        * Convert integer number to a string, using the given number
        * base, which must be between 2 and 16.
        */
        static String Val(int32 num, int32 base=10);
        static String Val(int64 num, int32 base=10);
        static String Val(uint32 unum, int32 base=10);
        static String Val(uint64 unum, int32 base=10);
        static String Val(float32 num, int32 prec=6, bool exp=false);
        static String Val(float64 num, int32 prec=6, bool exp=false);

        /// Convert string to a integer number, assuming given number base
        static int32 Int(const String& s, int32 base=10);
        static int32 Int(const char* s, int32 base=10);
        static uint32 UInt(const String& s, int32 base=10);
        static uint32 UInt(const char* s, int32 base=10);
        
        // TODO add conversion from string to int64
        static int64 Long(const String& s, int32 base=10);
        static int64 Long(const char* s, int32 base=10);
        static uint64 ULong(const String& s, int32 base=10);
        static uint64 ULong(const char* s, int32 base=10);
                
        /// Convert string into real number
        static float32 Float(const String& s);
        static float32 Float(const char* s);
        static float64 Double(const String& s);
        static float64 Double(const char* s);
        
        /**
        * Converts a single character into its 2 byte hex havlue
        * It will append the hex value to the out String
        */
        static void Hex(const unsigned char& c, String& out);
        /**
        * Converts the two hex characters into its interegal part
        * 
        */
        static char UnHex(const char& hex1, const char& hex2);
        // is character valid
        static bool IsValidHex(char c);
        /** is a valid character */
        static bool IsAThruZ(const char& c);
        static bool IsWhiteSpace(const char& c);
        static bool IsDigit(const char& c);
        
        /// Break Types/Object to bits and display the bits as a string
        /** returns binary form of bytes */
        static String Bits(const SRL::byte &num);
        /** returns binary form of int16 */
        static String Bits(const int16 &num);
        /** returns binary form of int32 */
        static String Bits(const int32 &num);
        /**  returns binary form of int64 */
        static String Bits(const int64 &num);

        /// Generate the hash value for the string
        static uint32 Hash(const String& s){ return s.hash(); }
        static uint32 Hash(const uint32& i){ return i; }
        static int32 Hash(const int32& i){ return i; }
        static int32 Hash(const float32& i){ return (int32)(i*100.0f); }

        /// Escape special characters in a string
        static String escape(const String& s);

        /// Unescape special characters in a string
        static String unescape(const String& s);

        /** represents a null string */
        static const char null[];
        /** storage for lowercase HEX */
        static const char hex[17];
        /** storage for uppercase HEX */
        static const char HEX[17];

    };
}
#endif //__SRL_String__
