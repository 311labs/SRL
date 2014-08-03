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
* Last $Author: ians $     $Revision: 216 $                                
* Last Modified $Date: 2005-12-28 19:52:10 -0500 (Wed, 28 Dec 2005) $                                  
******************************************************************************/
#ifndef __SRL_NonCopyable__
#define __SRL_NonCopyable__

#include "srl/exports.h"

namespace SRL
{
	/** Base Class to disable the copy constructor and assignment operator */
	class SRL_API NonCopyable
	{
	public:
		inline NonCopyable() {}
		virtual ~NonCopyable() {}
	private:
		NonCopyable(const NonCopyable &);
		NonCopyable &operator=(const NonCopyable &);
	};
}

#endif //__SRL_NonCopyable__

