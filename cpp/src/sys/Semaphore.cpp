#include "srl/sys/Semaphore.h"
#include "srl/sys/System.h"

#include <cstdlib>

using namespace SRL;
using namespace SRL::Errors;
using namespace SRL::System;

#ifdef __APPLE__
// For some unknown reason MAC OSX does not support unnamed semaphores via the sem_init/sem_destroy method set!
// Instead we need to do sem_open and sem_close... which we are now creating a work around for our problem by
// setting up this semaphore name counter
String GetSemName()
{
    // TODO: put mutex here to block this from problems
    static uint32 counter = 0;
    static uint32 proc_id = System::GetPid();
    ++counter;
    return String::Format("/tmp/SrlSem%d_%d", proc_id, counter);
}

#endif

#ifndef WIN32
#include <errno.h>
#endif

Semaphore::Semaphore(uint32 permits) : _count(0),  _permits(permits)
{
#ifdef WIN32
    _sem = ::CreateSemaphore((LPSECURITY_ATTRIBUTES)NULL, permits, MAX_SEM_VALUE, (LPCTSTR)NULL);
#elif defined(__APPLE__)
     /* Create a semaphore in locked state */
    _name = GetSemName();
    _sem = sem_open(_name.text(), O_CREAT, 0644, permits);
    if (_sem == SEM_FAILED)
    {
        if (errno == EINVAL)
            throw SemaphoreException("The sem_open() operation is not supported; or O_CREAT is specified and value exceeds SEM_VALUE_MAX.");
        else if (errno == ENOSPC)
            throw SemaphoreException("A resource required to initialise the semaphore has been exhausted, or the limit on semaphores (SEM_NSEMS_MAX) has been reached.");
        else if (errno == EACCES)
            throw SRL::Errors::PermissionDeniedException("The process lacks the appropriate privileges to initialise the semaphore.");      
        else
            throw SemaphoreException(System::GetLastErrorString());
    }
    
#else
    // create a semaphore structure, we do this instead of having one because of OSX and named semaphores
    _sem = (sem_t*)malloc(sizeof(sem_t));
    if (sem_init(_sem, PTHREAD_PROCESS_PRIVATE, permits)==-1)
    {
        if (errno == EINVAL)
            throw SemaphoreException("Attempting to create semaphore with to many permits");
        else if (errno == ENOSPC)
            throw SemaphoreException("A resource required to initialise the semaphore has been exhausted, or the limit on semaphores (SEM_NSEMS_MAX) has been reached.");
        else if (errno == ENOSYS)
            throw SRL::Errors::NotImplementedException("The function sem_init() is not supported by this implementation.");
        else if (errno == EPERM)
            throw SRL::Errors::PermissionDeniedException("The process lacks the appropriate privileges to initialise the semaphore.");
        throw SRL::Errors::UnknownException("unknown exception while call sem_init");
    }
#endif
}

Semaphore::~Semaphore()
{
#ifdef WIN32
	::CloseHandle(_sem);
#elif defined(__APPLE__)
	if (sem_unlink(_name.text()) == -1)
	{
	    throw SemaphoreException("Failed to remove the named semaphore");
	}
    sem_close(_sem);
#else
    ::sem_destroy(_sem);
    free(_sem);
#endif
}

void Semaphore::acquire()
{
#ifdef WIN32    
	if (::WaitForSingleObject(_sem, INFINITE) != WAIT_OBJECT_0)
	{
		throw SemaphoreException(System::GetLastErrorString());
	}
#else
	if (::sem_wait(_sem) != 0)
	{
		throw SemaphoreException(System::GetLastErrorString());
	}
#endif
}

bool Semaphore::tryAcquire()
{
#ifdef WIN32
	switch(::WaitForSingleObject(_sem, 0))
	{
	case WAIT_OBJECT_0:
        return true;
	case WAIT_TIMEOUT:
        return false;
    }
    return false;
#else
	return (::sem_trywait(_sem) == 0);
#endif
}

bool Semaphore::testAcquire()
{
    if (tryAcquire())
    {
        release();
        return true;
    }
    return false;
}

void Semaphore::release()
{
#ifdef WIN32
	::ReleaseSemaphore(_sem, 1, (LPLONG)NULL);
#else
    ::sem_post(_sem);
#endif
}

int Semaphore::availablePermits()
{
#ifndef WIN32
    int permits;
    if (sem_getvalue(_sem, &permits) == -1)
        throw SemaphoreException("sem_value");
    return permits;
#else
    return 0;
#endif

}

