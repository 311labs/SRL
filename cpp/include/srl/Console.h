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
#ifndef __SRL_console__
#define __SRL_console__

#include "srl/String.h"

namespace SRL
{
    
	/** Methods for reading and writing to standard out and standard err */
	namespace Console
	{
		/** write timestamp */
		SRL_API void writeDateTime();
		/** write end line */
		SRL_API void writeEndLine();
		/** write string to standard out */
		SRL_API void write(const String& output);
		/** write c string to standard out */
		SRL_API void write(const char* output);
		/** write a formatted string to stdout */
		SRL_API void format(const char* format,...);
				
		/** write a line to standard out */
		SRL_API void writeLine(const String& output);
		/** write a line to standard out */
		SRL_API void writeLine(const char* output);
		/** write a formatted string to stdout */
		SRL_API void formatLine(const char* format,...);

		/** read line from console */
		SRL_API void readLine(String& buffer);
		/** read line from console reading new string */
		SRL_API String readLine();
		/** read a character from the console */
		SRL_API char read();

		/** clear any text from the current screen */
		SRL_API void clear();
		
		namespace err
		{
			/** write timestamp */
			SRL_API void writeDateTime();
			/** write end line */
			SRL_API void writeEndLine();
			/** write string to standard out */
			SRL_API void write(const String& output);
			/** write c string to standard out */
			SRL_API void write(const char* output);
			/** write a formatted string to stdout */
			SRL_API void format(const char* format,...);
					
			/** write a line to standard out */
			SRL_API void writeLine(const String& output);
			/** write a line to standard out */
			SRL_API void writeLine(const char* output);
			/** write a formatted string to stdout */
			SRL_API void formatLine(const char* format,...);	
			/** This method dumps the current stack to stderr */
            SRL_API void dumpStack();
        	
		}
	}
}
#endif // __SRL_console__

