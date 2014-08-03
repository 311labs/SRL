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
#ifndef __SRL_Singleton__
#define __SRL_Singleton__

#include "srl/sys/Mutex.h"

namespace SRL
{
	/** Base Class for all singletons */
	template<class T>
	class Singleton
	{
	public:
		/** returns the instance of this class or creates one if it does not exist */
		static T *GetInstance()
		{
			return __getInstance();
		}
		/** destroys the instance of this class */
		static void FreeInstance()
		{
			__getInstance(true, true);
		}
		/** default destructor */
		virtual ~Singleton() {  }
	protected:
		Singleton<T>()
		{

		}

		//Singleton<T>(const Singleton &){}
		//Singleton &operator=(const Singleton &){}
		static T* __getInstance(bool kill=false,bool killed=false)
		{
			static T *__instance = NULL;
			static System::Mutex __sing_mutex;
			
			if ((__instance == NULL) || (kill))
			{
				System::ScopedLock lock(__sing_mutex);
				if (__instance == NULL)
				{
					__instance = new T;
				}
				else if (kill)
				{
					if (!killed)
						delete __instance;
					__instance = NULL;
				}
			}
			return __instance;
		}
	};
}

#endif //__SRL_Singleton__

