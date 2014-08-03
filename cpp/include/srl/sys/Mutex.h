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
#ifndef __SRL_Mutex__
#define __SRL_Mutex__

#include "srl/Exceptions.h"
#include "srl/sys/System.h"

namespace SRL
{
	namespace Errors
	{
		/** Thrown when is a mutex is not locked but unlock is called.  */
		class MutexException : public Exception
		{
		public:
			/** default constructor */
			MutexException(String message): Exception(message){};
		};	
	}
	
	namespace System
	{    
		
		/**
		* @author Ian Starnes
		*
		* @short A very simple class to wrap up critical sections
		* 
		* Mutex's or Mutexs are used when multiple threads need to change
		* the same block of memory.  They serialize entry into a particular section of code.
		* Calling lock() starts the serilization.  So each thread entering the lock() will not leave
		* the lock() until all threads before it have called unlock(). So you surround the code you
		* want to serialize with lock() and unlock(). Only one thread will execute the code in between 
		* those methods at a time and each thread will go in a first come first serve basis.
		* REMEMBER that you must call unlock() otherwise incoming threads will wait forever to
		* enter your protected logic and will never be able to run anything else.
		*
		* The Mutex(Mutex) is always recursive in that if the same thread invokes
		* the same mutex lock multiple times, it must release it multiple times.
		* This allows a function to call another function which also happens to
		* use the same mutex lock when called directly. This was
		* deemed essential because a mutex might be used to block individual file
		* restring2uests in say, a database, but the same mutex might be needed to block a
		* whole series of database updates that compose a "transaction" for one
		* thread to complete together without having to write alternate non-locking
		* member functions to invoke for each part of a transaction.
		*/
		class SRL_API Mutex  
		{  
		public:
			/** Contructor
			*/
			Mutex();
			/** Desctructor
			*/
			virtual ~Mutex();
			/** only locks if the mutex is not currently locked */
			bool tryLock();
			/** attempts a try lock within a certain time period */
			bool tryLock(uint32 timeout);
			/** Serializes all good until unlock is called
			* 
			* @param ms specifies a timeout value if a lock is not available
			* throws an exception if timeout occurs, -1 is infinite
			*/
			void lock();
			/** Stops serialization of code
			*/
			void unlock();
			/** returns the ID of the thread that has the lock or 0 
			*/
			SRL::THREAD_ID isLocked() const;
			/** returns the current lock count */
			uint32 lockCount() const;
			
		private:
			bool _is_locked;
			THREAD_ID _thread_id;
			uint32 _counter;
		#if defined(WIN32)
			CRITICAL_SECTION my_CS; // My Critical Section
		#else // linux
			pthread_mutex_t _pmutex; // My PThread Mutex
			pthread_mutexattr_t _mutexattr;
		#endif
		};
		/**
		* @short Locks a mutex for the life of the object
		*/
		class SRL_API ScopedLock
		{
		public:
			ScopedLock(Mutex &critsec) : _critsec(critsec)
			{
				_critsec.lock();
			};
			~ScopedLock()
			{
				_critsec.unlock();
			};
		private:
			Mutex &_critsec;
		};
	}
}

#endif // __SRL_Mutex__
