/**
* Written By Ian C. Starnes 2005
* *********************************
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either 
* version 2.1 of the License, or (at your option) any later version
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
***************************************************************
* Last $Author: ians $     $Revision: 168 $
* Last Modified $Date: 2005-10-10 01:40:38 -0400 (Mon, 10 Oct 2005) $
***************************************************************/
#ifndef __SRL_Serializable__ 
#define __SRL_Serializable__

#include "srl/io/BinaryWriter.h"
#include "srl/io/BinaryReader.h"

namespace SRL
{
    namespace IO
    {   
        //class BinaryReader;
        //class BinaryWriter;
    	class SRL_API Serializable
    	{
    	public:
    	    virtual ~Serializable();
    		virtual int serialize(BinaryWriter *writer) const=0;
    		virtual int deserialize(BinaryReader *reader)=0;
    	};
    }
}
#endif // __SRL_Serializable__

