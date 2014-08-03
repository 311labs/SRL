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
#ifndef __SRL_TEXTWRITER__
#define __SRL_TEXTWRITER__

#include "srl/String.h"
#include "srl/io/ByteBuffer.h"

namespace SRL
{
    namespace IO
    {
        /** Class for writing bytes or characters to a stream */
        class SRL_API TextWriter
        {
        public:
            virtual ~TextWriter(){}
            /** is the object ready to output data */
            virtual bool canWrite(int msec=SRL::NOW) const =0;
            /** write data to the output object. returns the number of bytes sent */
            virtual int write(const char* data, int len)=0;	

			/** write data to file */
			virtual int write(const SRL::String& str);
			/** write the SRL::String to the file and add an return('\n') */
			virtual int writeLine(const String& line);
			
			/** write data to file */
			virtual int format(const char* format, ...)=0;			
			/** write the formatted string to the file and add an return('\n') */
			virtual int formatLine(const char* format, ...)=0;

		    /** close the writer */
		    virtual void close()=0;
        };
    }
}

#endif // __SRL_TEXTWRITER__

