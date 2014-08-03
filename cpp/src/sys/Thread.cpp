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
* Last $Author: ians $     $Revision: 256 $                                
* Last Modified $Date: 2006-04-03 11:32:01 -0400 (Mon, 03 Apr 2006) $                                  
******************************************************************************/

#include "srl/sys/Thread.h"
#include "srl/Timer.h"

// TODO add SleepEx for windows and something similar for unix

#if defined(WIN32)
    #define MS_VC_EXCEPTION 0x406d1388
    #include <process.h>

    #ifdef _DEBUG
        #include <iostream>

        typedef struct tagTHREADNAME_INFO
        {
        DWORD dwType;  // must be 0x1000
        LPCSTR szName; // pointer to name (in user addr space)
        DWORD dwThreadID; // thread ID (-1=caller thread)
        DWORD dwFlags; // reserved for future use, must be zero
        } THREADNAME_INFO;

        static void SetThreadName( DWORD dwThreadID, LPCSTR szThreadName)
        {
            THREADNAME_INFO info;
            info.dwType = 0x1000;
            info.szName = szThreadName;
            info.dwThreadID = dwThreadID;
            info.dwFlags = 0;

        __try
            {
        RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD),
        (DWORD*)&info );
            }
            __except (EXCEPTION_CONTINUE_EXECUTION){}
        } 
    #endif
#endif // WIN32 Threads

using namespace SRL;
using namespace SRL::System;
using namespace SRL::Errors;


Thread::Thread(THREAD_PRIORITY priority, String name) :
_manager(NULL),
_thread_priority(priority),
_thread_name(name),
_thread_id(0),
_state(THREAD_DEAD),
_sleep_time(500),
_have_handle(false),
_do_pause(false),
_do_cancel(false),
_runnable(NULL),
_pauseSem(1),
_thread(0)
{

}

Thread::Thread(Runnable* runnable) :
_manager(NULL),
_thread_priority(THREAD_NORMAL),
_thread_name("SRL::Thread"),
_thread_id(0),
_state(THREAD_DEAD),
_sleep_time(500),
_have_handle(false),
_do_pause(false),
_do_cancel(false),
_runnable(runnable),
_pauseSem(1),
_thread(0)
{

}

Thread::Thread(ThreadManager* manager) :
_manager(manager),
_thread_priority(THREAD_NORMAL),
_thread_name("SRL::Thread"),
_thread_id(0),
_state(THREAD_DEAD),
_sleep_time(10),
_have_handle(false),
_do_pause(false),
_do_cancel(false),
_runnable(NULL),
_pauseSem(1),
_thread(0)
{

}


Thread::~Thread(void)
{
    // if thread is alive terminate
    if (isAlive())
    {
        // logger: thread destructor called but thread is still alive
        terminate();
    }
    // do any cleanup that may be left over
    _cleanUp();
}

void Thread::setPriority(THREAD_PRIORITY new_priority)
{
    _thread_priority = new_priority;
#if defined(WIN32)
    if (_have_handle)
        if (!(::SetThreadPriority(_thread, _thread_priority)))
            throw ThreadException("SetThreadPriority Failed");
#else
    /* sched_priority will be the priority of the thread */
    sched_param sp;
    sp.sched_priority = _thread_priority;
    /* only supported policy, others will result in ENOTSUP */

    int policy = SCHED_OTHER;
    /* scheduling parameters of target thread */
    int ret = pthread_setschedparam(_thread, policy, &sp);
    if (ret != 0)
        throw ThreadException(String::Format("SetThreadPriority Failed %d", ret));
#endif
}

void Thread::setSleep(uint64 milsec)
{
    // TODO validate this with tests.. .removed because 0 milsec works by not sleeping at all
    //if (milsec == 0)
    //    throw ThreadException("thread sleep can not be zero");
    _sleep_time = milsec;
}

void Thread::start()
{
    // if already started return false
    if (_have_handle)
    {
        if (_state >= THREAD_CLOSING)
        {
            //_threadlogger->error("thread id: %llu  - start called but is already running",_threadID);
            throw ThreadException("start called but thread is already running");
        }
        else
        {
            //_threadlogger->debug("thread id: %llu  - start called but thread is already started but not running! Will attempt to cleanup and restart",_threadID);
            _cleanUp();
        }
    }
    // reset the exit flag
    _do_cancel = false; 

    // create the thread
    _createThread();

    if (_thread_priority != THREAD_NORMAL)
        setPriority(_thread_priority);
}

void Thread::pause()
{
    if (!_do_pause)
    {
        _do_pause = true;
        _pauseSem.tryAcquire();
    }
}

void Thread::unpause()
{
    if (_do_pause)
    {
        _do_pause = false;
        if (_state == THREAD_PAUSED)
            _pauseSem.release();
    }
}

// DEPRECATED 
void Thread::stop(bool stop_now)
{
    if (stop_now)
        terminate();

    cancel();
}

void Thread::cancel(int32 milsec)
{
    // check if we can self terminate
    _selfTerminate();

    // make sure we are not paused 
    unpause();

    _do_cancel = true;
    if (milsec == NOW)
        return;

    if (join(milsec))
        _cleanUp();
    else
        throw ThreadDeadlock(String::Format("timed out waiting for cancel after (%d)", milsec));
}

void Thread::terminate()
{
    // check if we can self terminate
    _selfTerminate();
    // make sure we put our thread into cancel mode when it wake up
    _do_cancel = true;
    // make sure we are not paused 
    unpause();
#if defined(WIN32)
    _state = THREAD_CLOSING;
    TerminateThread(_thread, 0);
#else
    //pthread_kill(_thread, 9); //not sure why not to use this
    pthread_cancel(_thread);
    //if (r != 0)
    //  _threadlogger->error("pthread_cancel call failed with: %llu",r);
#endif
    // cleanup resources
    _cleanUp();
    // call final because the thread will not be able to
    final();
    _state = THREAD_DEAD;
}

bool Thread::join(uint64 milsec)
{
    if (_state >= THREAD_CLOSING)
    {
        // make sure we are not calling join on ourself
        if (System::CompareThreadIDs(_thread_id, System::GetCurrentThreadID()))
            throw ThreadException("calling thread is the same as thread wanting to join on! Can't wait for your own death!");
#if defined(WIN32)
        if (milsec == 0)
            milsec = INFINITE;
        if ((WaitForSingleObject(_thread, milsec) == WAIT_TIMEOUT) && (_state >= THREAD_CLOSING))
        {
            return false;
        }
#else // PTHREADS
        // if ms is infinite then lets just call pthread_join
        if (milsec == 0)
        {
            //_threadlogger->debug("thread id: %llu  - infinite wait for exit",_threadID);
            pthread_join(_thread, NULL);
            return true;
        }

        //setup a timer to measure our naps
        Timer timer;
        // figure out how long each nap should be
        uint64 naptime = milsec;
        if (naptime > 100)
            naptime = 100;
            
        timer.start();
        while ((_state >= THREAD_CLOSING) && (timer.totalMS() < milsec))
        {
            System::Sleep(naptime);
            //_threadlogger->debug("thread id: %llu  - wait for exit - remaining: %d",_threadID,(int) (ms - timer.totalMS()));
        }
        
        if (_state >= THREAD_CLOSING)
            return false;
#endif  
    }
    
    return true;
}

void Thread::_doPause()
{
    //printf("Thread::paused\n");
    _state = THREAD_PAUSED;
    _pauseSem.acquire();
    _pauseSem.release();
    //printf("Thread::unpaused\n");
}

void Thread::doStateCheck()
{
    if (_do_pause)
        _doPause();
    
    if (_do_cancel)
        _selfTerminate();
        
    _state = THREAD_BUSY;
}

//String Thread::strPriority() const
//{
//  THREAD_PRIORITY pri = GetCurrentThreadPriority();
//  switch (pri)
//  {
//      case THREAD_NORMAL:
//          return "NORMAL";
//      case SRL::THREAD_ABOVE_NORMAL:
//          return "ABOVER
//  }
//
//}

String Thread::toString() const
{
    return String::Format("Thread(%s:%u:%d)", name().text(), id(), priority());
}

// private logic
void Thread::_selfTerminate()
{
    if (System::CompareThreadIDs(_thread_id, System::GetCurrentThreadID()))
    {
        _state = THREAD_CLOSING;
        final();
        _state = THREAD_DEAD;  
    #if defined(WIN32)
        _endthreadex(0);
    #else
        pthread_exit(NULL);
    #endif  
    }
}
void Thread::_createThread()
{
#if defined(WIN32)
    // Standard Windows Threads
    uint32 id=-1;
    _thread = (HANDLE)_beginthreadex(NULL, 0, &__threadFunc, this, 0, &id );
    _thread_id = static_cast<uint32>(id);
    if (!_thread)
        throw ThreadException("_beginthreadex call failed");
    #ifdef _DEBUG || DEBUG_INFO
        // attempt to set the thread name for debugging
        SetThreadName(_thread_id, _thread_name.text());
    #endif

#else
    // assume we are creating PTHREADS
    if (pthread_create(&_thread, NULL, __threadFunc, this) != 0)
        throw ThreadException("pthread_create call failed");
    // setup the thread to cancel instantly
    //pthread_detach(_thread);
#endif
    _have_handle = true;
}

void Thread::_cleanUp()
{
    if (_have_handle)
    {
        //_threadlogger->debug("thread id: %llu  - begin cleanup",_threadID);
    #if defined(WIN32)
        CloseHandle(_thread);
    #else
        pthread_detach(_thread);
    #endif  //WIN32
        _have_handle = false;
        //_threadlogger->debug("thread id: %llu  - end cleanup",_threadID);
    }
}

// THE THREAD LOOP
#if defined (WIN32)
unsigned __stdcall Thread::__threadFunc(void* obj)
{
    Thread* aThread = (Thread*) obj;
    try
    {
        if (aThread->_manager == NULL)
            return aThread->__threadLoop();
        return aThread->__managedLoop();
    }
    catch(SRL::Errors::Exception &e)
    {
        aThread->thread_exception(e);
    }
    return 0;
}
#else
void* Thread::__threadFunc(void* obj)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    Thread* aThread = (Thread*) obj;
    try
    {
        if (aThread->_manager == NULL)
            aThread->__threadLoop();
        else
            aThread->__managedLoop();
        return NULL;
    }
    catch(SRL::Errors::Exception &e)
    {
        aThread->thread_exception(e);
    }
    return NULL;
}
#endif

uint64 Thread::__managedLoop()
{
    _state = THREAD_STARTING;
    _thread_id = System::GetCurrentThreadID();
    
    while (!_do_cancel)
    {
        if (_do_pause)
            _doPause();
        // each thread must start at the top
        if (initial())
        {
            // thread may be paused at start
            if (_do_pause)
                _doPause();       
        
            while (!_do_cancel)
            {   
                _state = THREAD_BUSY;
                // when a managed thread completes we return it to the manager
                if (!this->run())
                {
                    final();
                    _manager->task_completed(this);
                    break;
                }

                if (!_do_cancel)
                {
                    if (_do_pause)
                        _doPause();
                
                    _state = THREAD_SLEEPING;
                    System::Sleep(_sleep_time);
                }
            }
        } 
    }
    _state = THREAD_DEAD;
    return 0;    
}

uint64 Thread::__threadLoop()
{
    _state = THREAD_STARTING;
    _thread_id = System::GetCurrentThreadID();
    if (_do_pause)
        _doPause();
        
    if (initial())
    {
        // thread may be paused at start
        if (_do_pause)
            _doPause();       
        
        while (!_do_cancel)
        {   
            _state = THREAD_BUSY;
            // if run returns false exit thread
            if (!this->run())
                break;

            if (!_do_cancel)
            {
                if (_do_pause)
                    _doPause();
                
                _state = THREAD_SLEEPING;
                if (_sleep_time > 0)
                    System::Sleep(_sleep_time);
            }
        }
    }   
    _state = THREAD_CLOSING;
    final();
    _state = THREAD_DEAD;
    return 0;
}


