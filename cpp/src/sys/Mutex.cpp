
#include "srl/sys/Mutex.h"
#include "srl/Timer.h"

using namespace SRL;
using namespace SRL::System;

Mutex::Mutex() :
_is_locked(false),
_thread_id(NULL_THREAD),_counter(0)
{
    // This is where we initialize our Critical Section Logic
#if defined (WIN32)
    InitializeCriticalSection(&my_CS);
#else
    
    // setup a recursive mutex
    // this allows thread that holds the lock
    // to keep entering the lock 
    pthread_mutexattr_init(&_mutexattr);
    pthread_mutexattr_settype(&_mutexattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&_pmutex, &_mutexattr);
#endif
}

Mutex::~Mutex()
{
    // This is where we shutdown our Critical Section Logic
#if defined (WIN32)
    ::DeleteCriticalSection(&my_CS);
#else
    pthread_mutex_destroy(&_pmutex);
    //delete _pmutex;
#endif
    _counter=777;
    _thread_id=NULL_THREAD;

}

bool Mutex::tryLock()
{
#ifdef WIN32
    // We have now entered a critical section
    if (!::TryEnterCriticalSection(&my_CS))
        return false;
    _thread_id = System::GetCurrentThreadID();
#else
    THREAD_ID id = System::GetCurrentThreadID();    
    if (_thread_id != id)
    {
        if (pthread_mutex_trylock(&_pmutex) == 16)
            return false;
        _thread_id = id;
    }   
#endif  
    _is_locked = true;
    ++_counter; 
    return true;
}

bool Mutex::tryLock(uint32 milsec)
{
    bool flag = tryLock();
    if (!flag)
    {
        Timer timer;
        timer.start();
        
        while (!flag && (timer.totalMS() < milsec))
        {
            System::Yield();
            flag = tryLock();
        }
    }

    return flag;
}

void Mutex::lock()
{
#if defined (WIN32)
    // We have now entered a critical section
    ::EnterCriticalSection(&my_CS); 
    _thread_id = System::GetCurrentThreadID();
#else
    THREAD_ID id = System::GetCurrentThreadID();    
    if (_thread_id != id)
    {
        pthread_mutex_lock(&_pmutex);
        _thread_id = id;
    }
#endif  
    
    _is_locked = true;
    ++_counter;
}

void Mutex::unlock()
{
    THREAD_ID id = System::GetCurrentThreadID();    
    if (_thread_id != id)
        throw Errors::MutexException("Trying to unlock a mutex the thread does not own!");
    if (_counter == 0)
        throw Errors::MutexException("Trying to unlock a mutex that is not locked!");
    --_counter;
    // We are now leaving the critical section
    if (_counter == 0)
    {
        _is_locked = false;
        _thread_id = NULL_THREAD;
    }
    else if (_counter < 0)
    {
        throw Errors::MutexException("mutex count is less then 0!");
    }
#if defined (WIN32)
    ::LeaveCriticalSection(&my_CS);
#else
    if (_counter == 0)
        pthread_mutex_unlock(&_pmutex);
#endif
}

THREAD_ID Mutex::isLocked() const
{
    THREAD_ID id = System::GetCurrentThreadID();    
    if (_thread_id == id)
        return 0;
    return _thread_id;
}

uint32 Mutex::lockCount() const
{
    return _counter;
}
