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
#ifndef __SRL_Thread__
#define __SRL_Thread__

#include "srl/sys/Mutex.h"
#include "srl/sys/Semaphore.h"
#include "srl/sys/ThreadPool.h"

namespace SRL
{
    namespace Errors
    {
        /** Thrown own a thread logic error.  */
        class ThreadException : public Exception
        {
        public:
            /** default constructor */
            ThreadException(String message): Exception(message){};
        };  
        /** Thrown when the thread appears to be deadlocked */
        class ThreadDeadlock : public ThreadException
        {
        public:
            /** default constructor */
            ThreadDeadlock(String message): ThreadException(message){};
        };  
    }

    namespace System
    {


        /**
        * Interface used by any class that wants to be runnable by a Thread and not inherit a thread
        * This is useful if the instance is used with a thread pool
        */
        class SRL_API Runnable
        {
        public:
            virtual ~Runnable (){};


            /**
            * The initial method is called by a newly created thread when it
            * starts execution.  Put any logic in here that you wish to invoke before
            * the thread calls run() for the first time.
            * 
            * @see #run
            * @see #final
            */
            virtual bool initial(){return true;}

            /**
            * All threads execute by deriving the Run method of Thread.
            * This method is called after Initial to begin normal operation
            * of the thread.  If the method returns false, then the thread will
            * terminate after calling it's final() method.  If the method returns true
            * it will sleep for the setSleep duration and then call run again.
            *
            * @see #initial
            * @see #final
            * @see #setSleep
            */
            /** Your child class should override this method.  This is then called by 
            the thread after eash sleep
            */  
            virtual bool run() = 0;
    
            /**
            * This method is called when the thread is about to be terminated.  This
            * does not mean that the object will be destroyed just that the thread
            * will be terminated.  A new thread can be created by calling startThread again.
            * or you could put delete this to self destruct object.
            *
            * @see #run
            */
            virtual void final(){return;}
            
            /**
            * This method is called when the thread catches an unhandled exception.
            * After the exception is caught here the thread will abort and final
            * is not called.  The thread must then be restarted
            */
            virtual void thread_exception(SRL::Errors::Exception &e){ throw e; }
        protected:
            Runnable(){};
        };
        
        class ThreadManager;
        
        /* @short A base class used for creating threads
        *
        * This class is meant to be a base class and not used on its own.
        * The child child class must override the virtual method run().
        *
        * @warning User Must override run() for this to work
        * @todo expand this class to support thread priorities
        * @todo have the ability to turn on and off exception handling
        */
        class SRL_API Thread 
        {
        public:
            /** returns the current state of the thread */
            enum THREAD_STATE {
                THREAD_DEAD=-1, /**< state is not known */
                THREAD_CLOSING=-0,  /**< thread exited from thread loop but not stopped */
                THREAD_STARTING=1, /**< it has not been created yet */
                THREAD_SLEEPING, /**< it is now in sleep mode */
                THREAD_PAUSED, /**< it is now in sleep mode */
                THREAD_BUSY,  /**< currently inside the run method */
            };

            /** Creates the Thread */
            Thread(enum THREAD_PRIORITY priority=THREAD_NORMAL, String name="SRL::Thread");
            /** Create a Thread with the given Runnable */
            Thread(Runnable* runnable);
            /** Creates a thread that is managed by a ThreadPool */
            Thread(ThreadManager* manager);
            /** Destroys the Thread */
            virtual ~Thread();
    
            /** run the thread on this object instead */
            void setRunnable(Runnable* runnable){ _runnable = runnable; }
    
            /** returns the thread id */
            const THREAD_ID& id() const{ return _thread_id;}    
            /** Returns the name of the thread */
            const String& name() const{ return _thread_name;}
        
            /** returns the current state of the Thread
            * @see #THREAD_STATE */
            const THREAD_STATE& state() const{ return _state; }     
            /** returns true if the Thread is still alive */
            bool isAlive() const{return _state > 0;}
            /** returns true if the Thread is sleeping */
            bool isSleeping() const{return _state == THREAD_SLEEPING;}
            /** returns true if the Thread is paused */
            bool isPaused() const{return _state == THREAD_PAUSED;}
    
            /** This will actually create and start the thread. The thread does not run until this is called.    */
            void start();
            
            /** 
            * pause thread
            */
            void pause();
            /** unpause thread */
            void unpause();
            
            /** (DEPRECATED) This is unsafe.. call cancel or terminate */
            void stop(bool stop_now = false);
            /** Informs the thread to stop running when it gets a chance 
            * @param wait_mils  time in milliseconds to wait before timing out*/
            void cancel(int32 milsec=NOW);
            /** Informs the thread to terminate imediatelly (warning this is dangerous) */
            void terminate();
    
            /** will attempt to wait until thread stops running 
            * 
            * @param ms specifies a timeout value (0=infinite) 
            * returns true when thread has exited false on timeout
            */
            bool join(uint64 milsec=0);
            
            /** returns the current sleep time between run calls in Milliseconds */
            uint64 sleepTime() const{return _sleep_time;}
            /** Tell the Thread how long to sleep in between calls to run
            @param ms               Milliseconds to sleep.
            */
            void setSleep(uint64 milsec);
    

            
            /** Set the thread priority level 
            * @see #THREAD_PRIORITY */
            void setPriority(enum THREAD_PRIORITY priority);
            /** returns the current priority level */
            const THREAD_PRIORITY priority() const{return _thread_priority;} 
            
            /** returns the string that represents this thread (name, id, etc) */
            String toString() const;
            
            /** returns the string that represents the thread priority */
            //String strPriority() const;
    
        protected:
            /**
            * Internal callback method called when the thread is started 
            * It manages the thread run logic for unmanaged threads
            * @return     function succeeded
            */
            uint64 __threadLoop();
            
            /**
            * Internal callback method called when the thread is started 
            * It manages the thread run logic for managed threads
            * @return     function succeeded
            */
            uint64 __managedLoop();
            
            
        #if defined (WIN32)
            /** used when creating the thread.. ignore this
            @return     return of false will stop thread
            */  
            static unsigned __stdcall __threadFunc(void* obj);
        #else
            /** used when creating the thread.. ignore this
            @return     return of false will stop thread
            */  
            static void* __threadFunc(void* obj);
        #endif
            /**
            * All threads execute by deriving the Run method of Thread.
            * This method is called after Initial to begin normal operation
            * of the thread.  If the method returns false, then the thread will
            * terminate after calling it's final() method.  If the method returns true
            * it will sleep for the setSleep duration and then call run again.
            *
            * @see #initial
            * @see #final
            * @see #setSleep
            */
            /** Your child class should override this method.  This is then called by 
            the thread after eash sleep
            */  
            virtual bool run()
            { 
                if (_runnable != NULL) 
                    return _runnable->run(); 
                return false;
            }
    
            /**
            * This method is called when the thread is about to be terminated.  This
            * does not mean that the object will be destroyed just that the thread
            * will be terminated.  A new thread can be created by calling startThread again.
            * or you could put delete this to self destruct object.
            *
            * @see #run
            */
            virtual void final(){ if (_runnable != NULL) _runnable->final(); }
    
            /**
            * The initial method is called by a newly created thread when it
            * starts execution.  Put any logic in here that you wish to invoke before
            * the thread calls run() for the first time.
            * 
            * @see #run
            * @see #final
            */
            virtual bool initial()
            { 
                if (_runnable != NULL) 
                    return _runnable->initial(); 
                return true;
            }
    
    
            /**
            * This method is called when the thread catches an unhandled exception.
            * After the exception is caught here the thread will abort and final
            * is not called.  The thread must then be restarted
            */
            virtual void thread_exception(SRL::Errors::Exception &e){ throw e; }    
            /** check for pause or cancel
            * child should call this to check if pause or cancel is called
            * this will allow the thread to either pause or cancel during long operations */
            void doStateCheck();
            
            /** our thread pool manager */
            ThreadManager *_manager;
            
        private:
            /** self terminate.. a thread can call terminate on itself */
            void _selfTerminate();
            /** creates the thread */
            void _createThread();
            /** cleanup thread resources */
            void _cleanUp();
            /** pause thread */
            void _doPause();
    
            /** private thread variables */
            THREAD_PRIORITY _thread_priority;
            String _thread_name;
            THREAD_ID _thread_id;
            THREAD_STATE _state;
            uint32 _sleep_time;
            bool _have_handle;
            bool _do_pause, _do_cancel;
            Runnable *_runnable;
            
            Semaphore _pauseSem;
            
        #if defined (WIN32)
            HANDLE _thread;
        #else
            pthread_t _thread;
        #endif
        };
    }
}

#endif // SRL_THREA
