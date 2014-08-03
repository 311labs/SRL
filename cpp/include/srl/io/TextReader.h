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
#ifndef __SRL_TEXTREADER__
#define __SRL_TEXTREADER__

#include "srl/String.h"
#include "srl/io/ByteBuffer.h"

namespace SRL
{
    namespace IO
    {
        /** Class for reading bytes or characters from a stream */
        class SRL_API TextReader
        {
        public:
            virtual ~TextReader(){}
            /** is the object ready to be read from */
            virtual bool canRead(int msec=SRL::NOW) const =0;
            /** is at the end of the stream */
            virtual bool atEnd() const = 0;
            /** read 1 byte from the object */
            virtual char read() =0;
    		/** read data from the object. returns  the number of bytes read */
    		virtual int read(char* data, int len,  int msec = SRL::FOREVER) =0;

    		/** read len number of bytes into string, returns the amount read */
    		int read(String &buf, int len, int msec=SRL::FOREVER);    		
    		
    		/** read until a carriage return or line-feed is encountered */
            virtual String& readLine(String &line, bool strip_eol=true) =0;
    		/** read until cr or lf */
    		virtual String readLine(bool strip_eol=true) =0;

    		/** read the entire buffer */
    		virtual String readAll()=0;
    		/** read the entired buffer */
    		virtual String* readAll(String &in)=0;
    	    		
    		/** close the reader */
    		virtual void close()=0;
        };
    }
}

#endif // __SRL_TEXTREADER__

