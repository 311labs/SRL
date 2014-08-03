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
#ifndef __SRL_BINARYWRITER__
#define __SRL_BINARYWRITER__

#include "srl/String.h"

#ifdef write
#undef write
#endif

namespace SRL
{
    namespace IO
    {
        /** Class for writing bytes or characters to a stream */
        class SRL_API BinaryWriter
        {
        public:
            BinaryWriter(){};
            virtual ~BinaryWriter(){};
            /** is the object ready to output data */
            virtual bool canWrite(int msec=SRL::NOW)const =0;
    		/** write bytes to an IO Object (Must be overridden)*/
    		virtual int writeData(const void* obj, uint32 objsize, uint32 arrysize=1)=0;
  		
    		/** write byte to the buffer */
    		int write(const SRL::byte* value, uint32 arrysize=1);
    		/** write int16 to an IO Object */
    		int write(const SRL::int16 *obj, uint32 arrysize=1);
    		/** write int32 to an IO Object */
    		int write(const SRL::int32 *obj, uint32 arrysize=1);
    		/** write int64 to an IO Object */
    		int write(const SRL::int64 *obj, uint32 arrysize=1);
    		/** write uint16 to an IO Object */
    		int write(const SRL::uint16 *obj, uint32 arrysize=1);
    		/** write uint32 to an IO Object */
    		int write(const SRL::uint32 *obj, uint32 arrysize=1);
    		/** write uint64 to an IO Object */
    		int write(const SRL::uint64 *obj, uint32 arrysize=1);
    		/** write float32 to an IO Object */
    		int write(const float32 *obj, uint32 arrysize=1);
    		/** write float64 to an IO Object */
    		int write(const float64 *obj, uint32 arrysize=1);
    		/** write bool to an IO Object */
    		int write(const bool *obj, uint32 arrysize=1);
		    /** write a single byte */
            int write(const SRL::byte& b);
    		/** write string to an IO Object */
    		int write(const char *str, bool writeLength=true);
    		/** write String to an IO Object */
    		int write(const String &str, bool writeLength=true);
    		/** write a serilizable objects */
    		//int write(Serializable *object);
        };
    }
}

#endif // __SRL_BINARYWRITER__

