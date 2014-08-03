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
#ifndef __SRL_FILE__
#define __SRL_FILE__

#include "srl/fs/FsObject.h"

namespace SRL
{
    namespace FS
    {
		/** generic file base class */
		class SRL_API File : public FsObject
		{
		public:
			/** @deprecated file io types */
			enum MODE
			{
				ASCII_READ=0, ASCII_WRITE, ASCII_APPEND, 
				ASCII_READ_WRITE, ASCII_NEW_READ_WRITE,
				ASCII_READ_APPEND,
				BINARY_READ, BINARY_WRITE, BINARY_APPEND, 
				BINARY_READ_WRITE, BINARY_NEW_READ_WRITE,
				BINARY_READ_APPEND,
			};
			/** @deprecated  basic file constructor (with opening) */
			File(const String &filename, File::MODE io_type, bool newed=false);
			/** basic file constructor (with opening) */
			File(const String &filename, const char *io_mode, bool newed=false);
			/** basic file constructor (without opening) */
			File(const String &filename, bool newed=false);
			/** default destructor */
			~File();
	
			/** @deprecated set the current io mode */
			void setIoMode(File::MODE io_mode);
			/** set the current io mode */
			void setIoMode(char *io_mode);
	
			/** closes the file (deleting it if needed) */
			virtual void close();
			
			/** returns the current position in the file for use with setPosition
			* (do not use with seek method use setPosition instead) */
			uint32 position() const;
			/** returns the current position in the file for use with seek
			* (do not use with setPosition method use seek instead) */
			uint32 tell() const;
			/** returns true at beginning of file */
			Bool atBeginning() const;
			/** returns true at end of file */
			bool atEnd() const;
			/** returns true at EOF */
			Bool eof() const{return atEnd();}
	
			/** sets where our next read will be at */
			void setPosition(const uint32& pos);
			/** set the file position for the next read/write to the passed in
			* offset in bytes in the file */
			void seek(const int64 &offset, const int32 origin=SEEK_SET);	
	
			/** set the cursor to the front of the file */
			void gotoBeginning();
			/** legacy support */
			void rewind();
			/** set the cursor to the end of the file */
			void gotoEnd();
			/** this will return the number of bytes in the file */
			const uint64& size() const;
			/** tell the file to commit all changes */
			void commit();
			
		protected:
			/** default constructor */
			File();
			/** open the file */
			void _openFile();
			/** convert string io mode to enum */
			int find_io_mode(const char* io_mode);
			/** File Handle */
			FILE *_file;
			/** @deprecated current file io mode */
			int8 _io_mode;
			/** file io modes as string */
			static const char __io_modes[12][4];

			/** stores the size of the file data in bytes */
			mutable uint64 _file_data_size;
		};        
    }
}

#endif // __SRL_FILE__

