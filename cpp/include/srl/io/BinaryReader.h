/**
 * File: BinaryReader.h
 * Project: srl3
 * Created by: Ian Starnes on 2006-11-20.
 * 
 * This library is free software; you can redistribute it and/or                 
 * modify it under the terms of the GNU Lesser General Public                    
 * License as published by the Free Software Foundation; either                  
 * version 2.1 of the License, or (at your option) any later version.            
 *                                                                               
 * This library is distributed in the hope that it will be useful,               
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             
 * Lesser General Public License for more details.                               
 *                                                                               
 * You should have received a copy of the GNU Lesser General Public              
 * License along with this library; if not, write to the Free Software           
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.   
 */


#ifndef __SRL_BINARYREADER__
#define __SRL_BINARYREADER__

#include "srl/String.h"
//#include "srl/io/Serializable.h"

namespace SRL
{
    namespace IO
    {
        //class Serializable;
        /** Class for reading bytes or characters from a stream */
        class SRL_API BinaryReader
        {
        public:
            BinaryReader(){};
            virtual ~BinaryReader(){};            
            /** is the object ready to be read from */
            virtual bool canRead(int msec=SRL::NOW)const=0;
            /** is at the end of the stream */
            virtual bool atEnd() const = 0;
            /** read 1 byte from the object */
            virtual SRL::byte read()=0;
    		/** read bytes from an IO Object */
    		virtual int read(void* obj, uint32 objsize, uint32 arrysize=1)=0;
    		
    		/** read byte from the buffer */
    		int read(byte* value, uint32 arrysize=1);
    		/** read int16 from an IO Object */
    		int read(int16 *obj, uint32 arrysize=1);
    		/** read int32 from an IO Object */
    		int read(int32 *obj, uint32 arrysize=1);
    		/** read int64 from an IO Object */
    		int read(int64 *obj, uint32 arrysize=1);
    		/** read uint16 from an IO Object */
    		int read(uint16 *obj, uint32 arrysize=1);
    		/** read uint32 from an IO Object */
    		int read(uint32 *obj, uint32 arrysize=1);
    		/** read uint64 from an IO Object */
    		int read(uint64 *obj, uint32 arrysize=1);
    		/** read at32 from an IO Object */
    		int read(float32 *obj, uint32 arrysize=1);
    		/** read float64 from an IO Object */
    		int read(float64 *obj, uint32 arrysize=1);
    		/** read bool from an IO Object */
    		int read(bool *obj, uint32 arrysize=1);
		
    		/** read String from an IO Object */
    		int read(char *str, uint32 arrysize=0);
    		/** read String from an IO Object */
    		int read(String &str, uint32 arrysize=0);
    		/** read serializable object in */
    		//int read(Serializable *object);
        };
    }
}

#endif // __SRL_BINARYREADER__

