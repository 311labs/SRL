#include "srl/sys/ThreadPool.h"

using namespace SRL;
using namespace SRL::System;

ThreadPool::ThreadPool(const uint32& max_size, uint32 min_size) : 
_max_size(max_size),
_min_size(min_size),
_size(0),
_available_pool(),
_busy_pool(),
_pool_lock(),
_waiter(1),
_blocker(1)
{
    for(uint32 i=0; i < _min_size; ++i)
    {
        _available_pool.add(_createThread());
    }
}

ThreadPool::~ThreadPool()
{
    // first lets cancel all the busy threads
    for(uint32 i=0; i < _busy_pool.size(); ++i)
    {
        _busy_pool[i]->cancel();
    }
    
    // next lets cancel all our paused threads
    for(uint32 i=0; i < _available_pool.size(); ++i)
    {
        _available_pool[i]->cancel(500);
        delete _available_pool[i];
    }
    
    // now lets go back and terminate any busy threads still alive
    for(uint32 i=0; i < _busy_pool.size(); ++i)
    {
        if (_busy_pool[i]->isAlive())
            _busy_pool[i]->terminate();
        delete _busy_pool[i];
    }
    
    
}

void ThreadPool::runTask(Runnable* task)
{
    Thread* thread = _getThread();
    if (thread == NULL)
    {
        // no threads available, block until thread becomes available
        // acquire and release
        _blocker.acquire();
        thread = _getThread();
    }
    //printf("++ running %u\n", thread->id());
    thread->setRunnable(task);
    thread->unpause();
}

void ThreadPool::waitAll()
{
    if (_busy_pool.size() == 0)
        return;
        
    if (!_waiter.testAcquire())
    {
        _waiter.acquire();
        _waiter.release();
    }
}

Thread* ThreadPool::_getThread()
{
    _pool_lock.lock();
    Thread* thread = NULL;
    if (_available_pool.size() > 0)
    {
        thread = _available_pool.pop();
    }    
    else if (_size < _max_size)
    {
        thread = _createThread();
    }
    else
    {
        _pool_lock.unlock();
        return NULL;
    }
    
    // now if we have reached capacity start blocking incoming requests
    if ((_available_pool.size() == 0) &  (_size == _max_size))
    {
        // pool is maxed out lock the waiter semaphore
        _blocker.tryAcquire();
    }
    
    if (_busy_pool.size() == 0)
    {
        // we are the first active task lets acquire the waiter
        _waiter.tryAcquire();
    }
    
    _busy_pool.add(thread);
    _pool_lock.unlock();
    return thread;
}

void ThreadPool::task_completed(Thread* thread)
{
    ScopedLock lock(_pool_lock);
    //printf("enter task_completed %u\n", thread->id());
    // everything in this scope is now synchronized
    thread->pause();
    thread->setRunnable(NULL);
    
    _busy_pool.remove(thread);
    _available_pool.add(thread);
        
    if ((_available_pool.size() == 1) &  (_size == _max_size))
    {
        // pool is maxed out lock the waiter semaphore
        if (!_blocker.testAcquire())
            _blocker.release();
    }
    
    if (_busy_pool.size() == 0)
    {
        // there are no active tasks so lets release the waiter
        while (!_waiter.testAcquire())
            _waiter.release();
    }
    //printf("exiting task_completed %u\n", thread->id());
    
}

Thread* ThreadPool::_createThread()
{
    ++_size;
    Thread* t = new Thread(this);
    t->pause();
    t->start();
    return t;
}


