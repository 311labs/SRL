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
#ifndef __SRL_Semaphore__
#define __SRL_Semaphore__

#include "srl/sys/System.h"
#include "srl/Exceptions.h"

#ifndef WIN32
	#include <semaphore.h>
#else
	#define WIN32_LEAN_AND_MEAN
	#define _WIN32_WINNT 0x0501 
	#include "windows.h"
#endif

#define	MAX_SEM_VALUE	1000000

namespace SRL
{
	namespace Errors
	{
		/** Thrown when is a mutex is not locked but unlock is called.  */
		class SemaphoreException : public Exception
		{
		public:
			/** default constructor */
			SemaphoreException(String message): Exception(message){};
		};	
	}
	
	namespace System
	{    	
		/**
		* A semaphore is generally used as a synchronization object between multiple
		* threads or to protect a limited and finite resource such as a memory or
		* thread pool.  The semaphore has a counter which only permits access by
		* one or more threads when the value of the semaphore is non-zero.  Each
		* access reduces the current value of the semaphore by 1.  One or more
		* threads can wait on a semaphore until it is no longer 0, and hence the
		* semaphore can be used as a simple thread synchronization object to enable
		* one thread to pause others until the thread is ready or has provided data
		* for them.  Semaphores are typically used as a
		* counter for protecting or limiting concurrent access to a given
		* resource, such as to permitting at most "x" number of threads to use
		* resource "y", for example.   
		* 
		* @short Semaphore counter for thread synchronization. 
		*/
		class SRL_API Semaphore
		{
		public:
			/**
			* The initial value of the semaphore can be specified.  An initial
			* value is often used When used to lock a finite resource or to 
			* specify the maximum number of thread instances that can access a 
			* specified resource.
			* 
			* @param permits specify initial permit count or 0 default.
			*/
			Semaphore(uint32 permits = 0);
	
			/**
			* Destroying a semaphore also removes any system resources
			* associated with it.  If a semaphore has threads currently waiting
			* on it, those threads will all continue when a semaphore is
			* destroyed.
			*/
			virtual ~Semaphore();
	
			/**
			* Acquire is used to keep a thread held until the semaphore counter
			* is greater than 0.  If the current thread is held, then another
			* thread must increment the semaphore.  Once the thread is accepted, 
			* the semaphore is automatically decremented, and the thread 
			* continues execution.
			* 
			* @param permits specify the number of permits to aquire.
			*
			* @see #release
			*/
			void acquire();
	
			/**
			* tryAcquire is a non-blocking variant of acquire. If the semaphore counter
			* is greater than 0, then the thread is accepted and the semaphore
			* counter is decreased. If the semaphore counter is 0 tryAcquire returns
			* immediately with false.
			*
			* @return true if thread is accepted otherwise false
			*
			* @see #acquire
			* @see #release
			*/
			bool tryAcquire();
			
			
			/**
			* testAcquire is a non-blocking call that will try to acquire
			* a permit and if succesfull it will release it right away
			*
			*/
            bool testAcquire();
	
			/**
			* Releasing a semaphore increments its current value and releases
			* the first thread waiting for the semaphore if it is currently at
			* 0.  Interestingly, there is no support to increment a semaphore by
			* any value greater than 1 to release multiple waiting threads in
			* either pthread or the win32 API.  Hence, if one wants to release
			* a semaphore to enable multiple threads to execute, one must perform
			* multiple releases.
			* 
			* @see #wait
			*/
			void release();
			
			/**
			*
			*/
            int availablePermits();
	
		private:
            int _count, _permits;
		#if defined(WIN32)	
			HANDLE	_sem;
		#else
			sem_t* _sem;
		#endif
		
		#ifdef __APPLE__
            String _name;
        #endif
		};
	}
}

#endif // __SRL_Semaphore__
