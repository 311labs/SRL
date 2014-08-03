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
#ifndef __SRL_TXTFILE__
#define __SRL_TXTFILE__

#include "srl/io/TextReader.h"
#include "srl/io/TextWriter.h"
#include "srl/sys/System.h"

namespace SRL
{
    namespace FS
    {
		/** interface to ASCII files */
		class SRL_API FileStream : public IO::TextReader, public IO::TextWriter
		{
		public:
#ifdef WIN32
            /** create a file stream with an existing file handle */
            FileStream(HANDLE& hfile, int flags=0);
#endif
            /** create file io class from the passed in unix file descriptor */
            FileStream(int fd, const char* mode);
            /** create file io class with an existing FILE stream */
            FileStream(FILE* file);
            /** default destroctor */
            virtual ~FileStream();
			
			/** returns true at end of file */
			bool atEnd() const;
			
            /** is the object ready to be read from */
            bool canRead(int msec=SRL::NOW) const;
            /** read 1 byte from the object */
            char read();
    		/** read data from the object. returns  the number of bytes read */
    		int read(char* data, int len,  int msec = SRL::FOREVER);

    		/** read until a carriage return or line-feed is encountered */
            String& readLine(String &line, bool strip_eol=true);
    		/** read until cr or lf */
    		String readLine(bool strip_eol=true);

    		/** read the entire buffer */
    		String readAll();
    		/** read the entired buffer */
    		String* readAll(String &in);


            /** is the object ready to output data */
            bool canWrite(int msec=SRL::NOW) const;
            /** write data to the output object. returns the number of bytes sent */
            int write(const char* data, int len);	
			
			/** write data to file */
			int format(const char* format, ...);			
			/** write the formatted string to the file and add an return('\n') */
			int formatLine(const char* format, ...);

		    /** close the writer */
		    virtual void close();			
        protected:
			/** File Handle */
			FILE *_file;
            int _fd;
#ifdef WIN32
            HANDLE _hfile;
#endif	
		};        
    }
}

#endif // __SRL_TXTFILE__

