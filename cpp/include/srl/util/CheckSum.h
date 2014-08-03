/*********************************************************************************
* Copyright (C) 1999,2004 by srl, Inc.   All Rights Reserved.        *
*********************************************************************************
* Authors: Ian C. Starnes, Dr. J. Shane Frasier, Shaun Grant
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
*******************************************************************************                                                     
* Last $Author: ians $     $Revision: 184 $                                
* Last Modified $Date: 2005-10-17 18:26:59 -0400 (Mon, 17 Oct 2005) $                                  
******************************************************************************/

#ifndef __SRL_CHECKSUM__
#define __SRL_CHECKSUM__

#include "srl/types.h"
#include "srl/exports.h"
#include "srl/String.h"

namespace SRL
{
	namespace Util
	{
		// FIXME THESE SHOULD ALL BE CONST!!!
		// TODO shouldn't this be called MD5
		// IS THIS RSA's MD5 message digest algorithm?
		// or NIST's secure hash algorithm, SHA
		class SRL_API CheckSum
		{
		public:
			/** default constructor */
			CheckSum() : _sum(0)
			{ clear(); }
			/** default destructor */
			virtual ~CheckSum(){};
	
			/** clear the checksum */
			void clear() { _sum = 0; _c1 = 52845; _r = 55665; _c2 = 22719; }
			
			/** return the value of this checksum */
			uint32 value(){ return _sum; }
			/** fold to a 16 bit value */
			uint16 value16();
	
			void doIpCheckSum(uint16 *buf, uint32 len);
			/** add byte to checksum */
			void add(SRL::byte val);
			/** add int16 to checksum */
			void add(SRL::int16 val);
			/** add int32 to checksum */
			void add(SRL::int32 val);
			/** add int64 to checksum */
			void add(SRL::int64 val);
			/** add float32 to checksum */
			void add(SRL::float32 val);
			/** add float64 to checksum */
			void add(SRL::float64 val);
			/** add String to checksum */
			void add(SRL::String val);
			/** add byte to checksum */
			void add(char* val);
			/** add array of bytes */
			void add(SRL::byte* arry, int arry_size);
		protected:
			int32 _c1, _r, _c2;
			int32 _sum;
		};
	}
}
#endif // __SRL_CHECKSUM__
