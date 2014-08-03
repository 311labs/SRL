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

#ifndef __SRL_TYPES__
#define __SRL_TYPES__

#ifndef WIN32
	#include "srl/config.h"
#endif

#ifdef HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif // HAVE_SYS_TYPES_H

#define STL_SUPPORT

namespace SRL
{

#ifdef WIN32
	/** a 1 byte integer */
	typedef __int8 int8;
	/** a 2 byte integer */
	typedef __int16 int16;
	/** a 4 byte integer */
	typedef __int32 int32;
	/** a 8 byte integer */
	typedef __int64 int64;
	/** a unsigned 1 byte integer */
	typedef unsigned __int8 uint8;
	/** a unsigned 2 byte integer */
	typedef unsigned __int16 uint16;
	/** a unsigned 4 byte integer */
	typedef unsigned __int32 uint32;
	/** a 8 byte integer */
	typedef unsigned __int64 uint64;

#define SYSTEM_SEPERATOR "\\"
#define S_IRUSR _S_IREAD        /* Owner read permission */
#define S_IWUSR _S_IWRITE       /* Owner write permission */
#define S_IXUSR _S_IEXEC        /* Owner write permission */
#define S_IRGRP _S_IREAD        /* Group read permission */
#define S_IWGRP _S_IWRITE       /* Group write permission */
#define S_IXGRP _S_IEXEC        /* Owner write permission */
#define S_IROTH _S_IREAD        /* Other read permission */
#define S_IWOTH _S_IWRITE       /* Other write permission */
#define S_IXOTH _S_IEXEC        /* Owner write permission */


#elif defined(__APPLE__)&&(__MACH__)
    #define OSX 1
	/** a 1 byte integer */
	typedef int8_t int8;
	/** a 2 byte integer */
	typedef int16_t int16;
	/** a 4 byte integer */
	typedef int32_t int32;
	/** a 8 byte integer */
	typedef int64_t int64;
	/** a unsigned 1 byte integer */
	typedef u_int8_t uint8;
	/** a unsigned 2 byte integer */
	typedef u_int16_t uint16;
	/** a unsigned 4 byte integer */
	typedef u_int32_t uint32;
	/** a 8 byte integer */
	typedef u_int64_t uint64;
#define SYSTEM_SEPERATOR "/"

#elif defined(linux)
	/** a 1 byte integer */
	typedef __int8_t int8;
	/** a 2 byte integer */
	typedef __int16_t int16;
	/** a 4 byte integer */
	typedef __int32_t int32;
	/** a 8 byte integer */
	typedef __int64_t int64;
	/** a unsigned 1 byte integer */
	typedef __uint8_t uint8;
	/** a unsigned 2 byte integer */
	typedef __uint16_t uint16;
	/** a unsigned 4 byte integer */
	typedef __uint32_t uint32;
	/** a 8 byte integer */
	typedef __uint64_t uint64;
#define SYSTEM_SEPERATOR "/"

#else
	// UNKNOWN OS
	#error "unknown platform!"
#endif // linux

/** a 4 byte float */
typedef float float32;
/** a 8 byte float */
typedef double float64;
/** standard floating point type */
typedef float64 real;
/** standard fixed point type */
typedef int32 integer;
/** standard unsigned fixed point type */
typedef uint32 uinteger;
/** a single byte */
typedef uint8 byte;

/** define type bool as one byte */
typedef byte Bool;

/** define true */
#ifdef TRUE
#undef TRUE
#endif
const Bool TRUE = 1;

/** define false */
#ifdef FALSE
#undef FALSE
#endif
const Bool FALSE = 0;

/** simple constant */
#ifdef INVALID
#undef INVALID
#endif
const SRL::int8 INVALID = -1;
/** flags used for wait states */

const SRL::int8 FOREVER = -1;
const SRL::int8 NOW = 0;

/// SIMPLE MATH
#define MIN(a, b)  (((a) < (b)) ? (a) : (b)) 
#define MAX(a, b)  (((a) > (b)) ? (a) : (b)) 

// Round up to nearest roundval
#define ROUNDUP(n, roundval)  (((n)+roundval-1)&-roundval)

////////// Checking printf and scanf format strings
#if defined(_CC_GNU_) || defined(__GNUG__) || defined(__GNUC__)
#define SRL_PRINTF(fmt,arg) __attribute__((format(printf,fmt,arg)))
#define SRL_SCANF(fmt,arg)  __attribute__((format(scanf,fmt,arg)))
#else
#define SRL_PRINTF(fmt,arg)
#define SRL_SCANF(fmt,arg)
#endif // PRINTF

#ifndef Reminder
/* Statements like:
//		#pragma message(Reminder "Fix this problem!")
// Which will cause messages like:
//		C:\Source\Project\main.cpp(47): Reminder: Fix this problem!
// to show up during compiles.  Note that you can NOT use the
// words "error" or "warning" in your reminders, since it will
// make the IDE think it should abort execution.  You can double
 * click on these messages and jump to the line in string2uestion.*/
#define Stringize( L )			#L
#define MakeString( M, L )		M(L)
#define $Line					\
	MakeString( Stringize, __LINE__ )
#define Reminder				\
	__FILE__ "(" $Line ") : Reminder: " 
#define reminder Reminder
#endif

// End Of Line
#ifdef WIN32
	#define EOL_STRING "\r\n"
#else
	#define EOL_STRING "\n"
#endif

    /** big endian test */
    inline bool IsBigEndian() 
    {
    	static uint16 test=0x1234;
    	return (*reinterpret_cast<uint8*>(&test)) == 0x12;
    }

    /** stores a time value */
    struct TimeValue
    {
    	// FIXME tv_sec should probably be int64
    	// seconds
    	int32 tv_sec; 
    	// microseconds
    	int32 tv_usec;
    	// timezone
    	uint8 tv_zone;
    	// daylight savings time
        uint8 tv_dst;
    };

}

#endif //__SRL_TYPES__

