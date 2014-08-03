
#include "ThreadTest.h"
#include "srl/Timer.h"
#include "srl/sys/Thread.h"
#include <math.h>

#include "srl/Console.h"

using namespace SRL::Test;
using namespace SRL;

// THREAD LOGIC
bool BasicThread::run()
{
	++_counter;
	if (_counter == 20)
		return false; // this exits automagically
	return true; 
}

bool ThreadTest::basic_thread_test(BasicThread *t1)
{
	const uint32 sleep_time = 20;
	// set thread to sleep for 100ms between counter increments
	t1->setSleep(sleep_time);
	t1->_counter = 0;

	// test the thread state
	TEST_ASSERT(t1->sleepTime() == sleep_time);
	TEST_ASSERT(t1->isAlive() == false);
	TEST_ASSERT(t1->isSleeping() == false);
	TEST_ASSERT(t1->priority() == SRL::System::THREAD_NORMAL);
	TEST_ASSERT(t1->state() == SRL::System::Thread::THREAD_DEAD);
	TEST_ASSERT(t1->counter() == 0);

	// start the thread
	t1->start();
	// yield so the thread can start
	System::Sleep(sleep_time+1);
	// test the thread state now that its started
	TEST_ASSERT(t1->sleepTime() == sleep_time);
	TEST_ASSERT(t1->isAlive() == true);
	TEST_ASSERT(t1->priority() == SRL::System::THREAD_NORMAL);
	TEST_ASSERT(t1->state() >= SRL::System::Thread::THREAD_CLOSING);
	TEST_ASSERT(t1->counter() > 0);
	// wait for the thread to die for no longer then 5 seconds
	TEST_ASSERT(t1->join(5000)== true);

	// test the thread state again
	TEST_ASSERT(t1->sleepTime() == sleep_time);
	TEST_ASSERT(t1->isAlive() == false);
	TEST_ASSERT(t1->isSleeping() == false);
	TEST_ASSERT(t1->priority() == SRL::System::THREAD_NORMAL);
	TEST_ASSERT(t1->state() == SRL::System::Thread::THREAD_DEAD);
	TEST_ASSERT(static_cast<uint32>(t1->counter()) == sleep_time);

	t1->stop(true);
	return true;
}

bool ThreadTest::basicThread()
{
	// basic thread on the stack
 	BasicThread t1;
	// run the basic thread test 10
	for (int i=0;i<3;++i)
	{
		if (!basic_thread_test(&t1))
		    return false;
	}



	// basic thread on the heap
	bool flag = true;
	BasicThread *t2 = new BasicThread();
	// run the basic thread test 10
	for (int i=0;i<3;++i)
	{
		if (!basic_thread_test(t2))
		{
		    flag = false;
		    break;
	    }
	}
	delete t2;


    // catching exceptions in the thread
    ErrorThread et;
    et.start();
    System::Sleep(200);
    TEST_ASSERT(et.err_msg == "test thread errors");
    // TODO add more thread error tests

    return flag;
}

// BASIC MUTEX TESTING
bool MutexThread::initial()
{
	_mutex.lock();
	return true;
}

bool MutexThread::run()
{
	++_counter;
	if (_counter == 5)
	{
		return false;
	}
	return true;
}

void MutexThread::final()
{
	_mutex.unlock();
}

bool ThreadTest::basic_mutex_test(MutexThread *t1)
{
	const int sleep_time = 50;
	// mutex should not be locked for us
	TEST_ASSERT(!t1->_mutex.isLocked());
	// test if we can lock the mutex ourselves
	t1->_mutex.lock();
	// mutex should not be locked for us
	TEST_ASSERT(!t1->_mutex.isLocked());
	// now we should be able to lock it again
	t1->_mutex.lock();
	// now check the lock count should be 2
	TEST_ASSERT(t1->_mutex.lockCount() == 2);
	// now we should unlock it once and it should still be locked
	t1->_mutex.unlock();
	// now check the lock count should be 1
	TEST_ASSERT(t1->_mutex.lockCount() == 1);	
	// now we should unlock it once and it should no longer be locked
	t1->_mutex.unlock();
	// now check the lock count should be 0
	TEST_ASSERT(t1->_mutex.lockCount() == 0);	

	t1->setSleep(sleep_time);
	t1->_counter = 0;
	// test to make sure the mutex is unlocked
	TEST_ASSERT(!t1->_mutex.isLocked());
	// start mutex thread
	t1->start();
	// give the mutex thread time to start
	System::Sleep(sleep_time+10);
	// test the mutex is now locked
	TEST_ASSERT(t1->_mutex.isLocked());
	// lock count should be 1
	TEST_ASSERT(t1->_mutex.lockCount() == 1);	
	// test the try lock for failure
	TEST_ASSERT(!t1->_mutex.tryLock(25));
	// now test the try lock for success
	TEST_ASSERT(t1->_mutex.tryLock(3000));
	// test to make sure the mutex is unlocked
	TEST_ASSERT(t1->_mutex.lockCount() == 1);
	// unlock the mutex
	t1->_mutex.unlock();
	TEST_ASSERT(t1->_mutex.lockCount() == 0);
	// test to make sure the thread is dead
	TEST_ASSERT(!t1->isAlive());
	
	return true;
}

bool ThreadTest::basicMutex()
{
	// basic thread on the stack
 	MutexThread t1;
	// run the basic mutex test
	for (int i=0;i<3;++i)
	{
		if (!basic_mutex_test(&t1))
		    return false;
	}

	// basic thread on the heap
	bool flag = true;
	MutexThread *t2 = new MutexThread();
	// run the basic mutex test
	for (int i=0;i<3;++i)
	{
		if (!basic_mutex_test(t2))
		{
		    flag = false;
		    break;
		}
	}
	delete t2;
	
	return flag;
}

// BASIC SEMAPHORE TESTS
bool SemaphoreThread::initial()
{
	_sem->acquire();
	return true;
}


bool SemaphoreThread::run()
{
	++_counter;
	if (_counter >= 10)
		return false;
	return true;
}

void SemaphoreThread::final()
{
	_sem->release();
}


bool ThreadTest::basicSemaphore()
{
	// create a semaphore that allows one in at a time
	SRL::System::Semaphore semaphore(1);
	SemaphoreThread t1(&semaphore), t2(&semaphore), t3(&semaphore), t4(&semaphore);
	
    //printf("permits: %d", semaphore.availablePermits());
	TEST_ASSERT(semaphore.testAcquire());
	//printf("permits: %d", semaphore.availablePermits());
	t1.start();
	SRL::System::Sleep(100);
	TEST_ASSERT(!semaphore.tryAcquire());
	t2.start();t3.start();t4.start();
	SRL::System::Sleep(120);
	TEST_ASSERT(t2._counter == 0);
	TEST_ASSERT(t3._counter == 0);
	TEST_ASSERT(t4._counter == 0);
	// now wait until all have completed
	for (int i=0;i<50;++i)
	{
		SRL::System::Sleep(100);
		if (!t1.isAlive() && !t2.isAlive() && !t3.isAlive() && !t4.isAlive())
			break;
	}

    //printf("%d\n", t1.state());
	TEST_ASSERT(!t1.isAlive() && !t2.isAlive() && !t3.isAlive() && !t4.isAlive());
	TEST_ASSERT(t1._counter == 10);
	TEST_ASSERT(t2._counter == 10);
	TEST_ASSERT(t3._counter == 10);
	TEST_ASSERT(t4._counter == 10);

	// now lets test more then one at a time
	SRL::System::Semaphore semaphore2(2);
	t1.setSem(&semaphore2);
	t2.setSem(&semaphore2);
	t3.setSem(&semaphore2);
	t4.setSem(&semaphore2);

	t1.start();t2.start();
	SRL::System::Sleep(120);
	TEST_ASSERT(t2._counter != 0);
	TEST_ASSERT(!semaphore2.tryAcquire());
	t3.start();t4.start();
	SRL::System::Sleep(120);
	TEST_ASSERT(t3._counter == 0);
	TEST_ASSERT(t4._counter == 0);

	// now wait until all have completed
	for (int i=0;i<50;++i)
	{
		SRL::System::Sleep(100);
		if (!t1.isAlive() && !t2.isAlive() && !t3.isAlive() && !t4.isAlive())
			break;
	}

	TEST_ASSERT(!t1.isAlive() && !t2.isAlive() && !t3.isAlive() && !t4.isAlive());
	TEST_ASSERT(t1._counter == 10);
	TEST_ASSERT(t2._counter == 10);
	TEST_ASSERT(t3._counter == 10);
	TEST_ASSERT(t4._counter == 10);
	
	// now lets test pausing, which uses a semaphore to pause the thread
    t1.setSem(&semaphore);
    t1.pause();
    t1.start();
    SRL::System::Sleep(120);
    TEST_ASSERT(t1._counter == 0);
    TEST_ASSERT(t1.state() == SRL::System::Thread::THREAD_PAUSED);
    t1.unpause();
    SRL::System::Sleep(120);
    TEST_ASSERT(t1._counter != 0);
    
    for (int i=0;i<50;++i)
    {
        SRL::System::Sleep(100);
        if (!t1.isAlive())
            break;
    }   
    
    // now lets test stopping a paused thread
    t1.setSem(&semaphore);
    t1.pause();
    t1.start();
    SRL::System::Sleep(120);
    TEST_ASSERT(t1._counter == 0);
    TEST_ASSERT(t1.state() == SRL::System::Thread::THREAD_PAUSED);
    t1.terminate();
    TEST_ASSERT(t1.state() == SRL::System::Thread::THREAD_DEAD);

    return true;
}

// Test Thread Canceling

bool LongThread::run()
{
	++_counter;
	return true;
}

bool BusyThread::run()
{
	double val1 = -3.1432;
	double val2 = 6.123121432;
	double result = 0.0;
	for (int x = 0; x < 1000000; ++x)
	{
			result += atan(fabs(((val1*val2)+(val1/sqrt(val2))*2.0))-1);
			++_counter;
			if (x % 10000 == 0)
			{
				System::Sleep(10);
			}
	}
	return true;
}

bool ThreadTest::cancelThread()
{
	LongThread t1;
	t1.setSleep(100);

	// test basic cancel
	t1.start();
	System::Sleep(20);
	TEST_ASSERT(t1.isAlive());
	t1.cancel();
	System::Sleep(200);
	TEST_ASSERT(!t1.isAlive());
	
	// test delayed cancel
	t1.start();
	System::Sleep(20);
	TEST_ASSERT(t1.isAlive());
	t1.cancel(200);
	TEST_ASSERT(!t1.isAlive());

	// test busy cancel
	BusyThread t2;
	t2.setSleep(10);
	t2.start();
	System::Sleep(20);
	TEST_ASSERT(t2.state() == SRL::System::Thread::THREAD_BUSY);
	TEST_THROWS(t2.cancel(1), SRL::Errors::ThreadDeadlock);
	TEST_THROWS_NOTHING(t2.cancel(10000));
	TEST_ASSERT(!t1.isAlive());

	// rerun the test
	t2.start();
	System::Sleep(20);
	TEST_ASSERT(t2.state() == SRL::System::Thread::THREAD_BUSY);
	TEST_THROWS(t2.cancel(10), SRL::Errors::ThreadDeadlock);
	TEST_THROWS_NOTHING(t2.cancel(10000));
	TEST_ASSERT(!t2.isAlive());
	return true;
}

bool ThreadTest::terminateThread()
{
	// test busy terminate
	BusyThread t2;
	t2.setSleep(10);
	t2.start();
	System::Sleep(20);
	TEST_ASSERT(t2.state() == SRL::System::Thread::THREAD_BUSY);
	t2.terminate();
	TEST_ASSERT(!t2.isAlive());

	// rerun
	t2.start();
	System::Sleep(20);
	TEST_ASSERT(t2.state() == SRL::System::Thread::THREAD_BUSY);
	t2.terminate();
	TEST_ASSERT(!t2.isAlive());
	return true;
}

// Check doStateCheck method
bool BusyStateThread::run()
{
	double val1 = -3.1432;
	double val2 = 6.123121432;
	double result = 0.0;
	for (int x = 0; x < 1000000; ++x)
	{
			result += atan(fabs(((val1*val2)+(val1/sqrt(val2))*2.0))-1);
			++_counter;
			if (x % 10000 == 0)
			{
				doStateCheck();
				System::Sleep(10);
				doStateCheck();
			}
	}
	return true;
}

bool ThreadTest::stateCheckThread()
{
	BusyStateThread t1;
	t1.setSleep(10);
	t1.start();
	System::Sleep(20);
	TEST_ASSERT(t1.isAlive());
	TEST_THROWS_NOTHING(t1.cancel(40));
	TEST_ASSERT(t1.isAlive() == false);
	return true;
}

bool ThreadTest::managedThreads()
{
    System::ThreadPool pool(5);
    TestTask task[8];
    
    for(uint32 i=0; i < 8; ++i)
    {
        pool.runTask(&task[i]);
    }

    System::Sleep(500);
    TEST_ASSERT(task[0].counter > 0);
    TEST_ASSERT(task[1].counter > 0);
    TEST_ASSERT(task[2].counter > 0);
    TEST_ASSERT(task[6].counter > 0);
    pool.waitAll();
    TEST_ASSERT(task[7].finished == true);
	
	return true;
}
