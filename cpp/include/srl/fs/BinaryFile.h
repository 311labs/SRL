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
#ifndef __SRL_BINFILE__
#define __SRL_BINFILE__

#include "srl/fs/File.h"
#include "srl/io/TextReader.h"
#include "srl/io/TextWriter.h"

namespace SRL
{
    namespace FS
    {
		/** interface to ASCII files */
		class SRL_API BinaryFile : public File, public IO::BinaryReader, public IO::BinaryWriter
		{
		public:
			/** @deprecated  basic file constructor (with opening) */
			BinaryFile(const String &filename, File::MODE io_type, bool newed=false) : File(filename, io_type, newed){};
			/** basic file constructor (with opening) */
			BinaryFile(const String &filename, const char *io_mode, bool newed=false) : File(filename, io_mode, newed){};
			
            /** is the object ready to be read from */
            bool canRead(int msec=SRL::NOW) const;
            /** is at the end of the stream */
            bool atEnd() const{ return File::atEnd();}
            /** read 1 byte from the object */
            SRL::byte read();
    		/** read bytes from an IO Object */
    		int read(void* obj, uint32 objsize, uint32 arrysize=1);

            /** is the object ready to output data */
            bool canWrite(int msec=SRL::NOW) const;
    		/** write bytes to an IO Object (Must be overridden)*/
    		int write(const void* obj, uint32 objsize, uint32 arrysize=1);
            inline int writeData(const void* obj, uint32 objsize, uint32 arrysize=1){return write(obj, objsize, arrysize);}

		    /** close the writer */
		    void close();			
		
		};        
    }
}

#endif // __SRL_BINFILE__

