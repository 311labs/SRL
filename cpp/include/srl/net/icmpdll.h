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

/******************************************************************************
* Microsoft ICMP DLL API
******************************************************************************/

#ifndef _ICMP_DLL_INCLUDED_
#define _ICMP_DLL_INCLUDED_

#include "srl/net/IcmpMessage.h"

namespace SRL
{
	namespace Net
	{
		namespace ICMP
		{

			/** send echo request */
			extern SRL_API Response IcmpDllEchoRequest(const InetSocketAddress &addr, 
				const uint32 &sequence, SRL::uint16 msg_size=56, SRL::uint32 timeout=5000, uint32 ttl=200);
		    
		    
		}
	}
}

#endif