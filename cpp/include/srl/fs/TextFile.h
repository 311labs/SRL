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

#include "srl/fs/File.h"
#include "srl/io/TextReader.h"
#include "srl/io/TextWriter.h"

namespace SRL
{
    namespace FS
    {
		/** interface to ASCII files */
		class SRL_API TextFile : public File, public IO::TextWriter, public IO::TextReader
		{
		public:
			/** @deprecated  basic file constructor (with opening) */
			TextFile(const String &filename, File::MODE io_type, bool newed=false) : File(filename, io_type, newed){};
			/** basic file constructor (with opening) */
			TextFile(const String &filename, const char *io_mode, bool newed=false) : File(filename, io_mode, newed){};
			/** returns true at end of file */
			bool atEnd() const { return File::atEnd();};
			
            /** is the object ready to be read from */
            bool canRead(int msec=SRL::NOW) const;
            /** read 1 byte from the object */
            char read();
    		/** read data from the object. returns  the number of bytes read */
    		int read(char* data, int len, int msec = SRL::FOREVER);
            int read(String &buf, int len, int msec = SRL::FOREVER);
    		/** read until a carriage return or line-feed is encountered */
            String& readLine(String &line, bool strip_eol=true);
    		/** read until cr or lf */
    		String readLine(bool strip_eol=true);

    		/** read the entire buffer */
    		String readAll();
    		/** read the entired buffer */
    		String* readAll(String &in);

			/** scanf function */
			void scanf(const char *format, ...);
			/** vscanf function */
			void vscanf(const char *format, va_list arg);

			/** blocks until a newline is written to the file */
			String tail();
			/** blocks until a newline is written to the file */
			String* tail(String &result);
						
            /** is the object ready to output data */
            bool canWrite(int msec=SRL::NOW) const;
            /** write data to the output object. returns the number of bytes sent */
            int write(const char* data, int len);	
			
			int write(const String& data){ return IO::TextWriter::write(data); }
			
			/** write data to file */
			int format(const char* format, ...);			
			/** write the formatted string to the file and add an return('\n') */
			int formatLine(const char* format, ...);

		    /** close the writer */
		    virtual void close();			
		
		};
		
		// Some usefull public functions
		/**(deprecated) create a new file for writing, close will free the object */
		extern SRL_API TextFile* CreateTextFile(const String &filename, bool overwrite=false);
		/**(deprecated) open an existing file */
		extern SRL_API TextFile* OpenTextFile(const String &filename, bool for_writing=false);
		/** write data to a file */
		extern SRL_API void WriteTextFile(const SRL::String &filename, const SRL::String &text, bool overwrite=false);
		/** write data to a file */
		extern SRL_API void AppendToTextFile(const String &filename, const String &text);
		/** opens a text file, reads all the text into the result string */
		extern SRL_API void CatTextFile(const String &filename, String *result);
		/** opens a text file and returns all of the text as a string */
		extern SRL_API String Cat(const String &filename);		
		/** Scanf the text file */
		extern SRL_API void ScanTextFile(const String &filename, const char* format, ...);
				       
    }
}

#endif // __SRL_TXTFILE__



