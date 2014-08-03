#ifndef __THREAD_TEST__
#define __THREAD_TEST__

#include "srl/sys/Thread.h"
#include "srl/Test.h"

namespace SRL
{
   namespace Test
   {
    /** Simple Thread */
    class BasicThread : public SRL::System::Thread
    {
    public:
    	BasicThread() : _counter(0){};
    	SRL::int32 counter() const{return _counter;}
    	/** our local mutex */
    	SRL::System::Mutex _mutex;
    	/** our local counter */
    	SRL::int32 _counter;
    protected:
    	/** our inherited run method */
    	bool run();
    };
    
    class ErrorThread: public SRL::System::Thread
    {
    public:
        ErrorThread(): err_msg("nothing"){}
    	/** our inherited run method */
        bool run(){ throw SRL::Errors::IOException("test thread errors"); }

        String err_msg;
        void thread_exception(SRL::Errors::Exception &e){ err_msg = e.message(); }
    };

    class MutexThread : public BasicThread
    {
    protected:
    	/** our initial method */
    	virtual bool initial();
    	/** our inherited run method */
    	virtual bool run();
    	/** our final method */
    	virtual void final();
    };

    class SemaphoreThread : public SRL::System::Thread
    {
    public:
    	SemaphoreThread(SRL::System::Semaphore *sema) : _sem(sema), _counter(0) { setSleep(100);}
    	SRL::System::Semaphore *_sem;
    	int _counter;
    	/** set our semaphore */
    	void setSem(SRL::System::Semaphore *sema){ _counter = 0; _sem = sema; }

    	/** our initial method */
    	virtual bool initial();
    	/** our inherited run method */
    	virtual bool run();
    	/** our final method */
    	virtual void final();
    };

    class LongThread : public BasicThread
    {
    protected:
    	/** our inherited run method */
    	virtual bool run();
    };

    class BusyThread : public BasicThread
    {
    protected:
    	/** our inherited run method */
    	virtual bool run();
    };

    class BusyStateThread : public BasicThread
    {
    protected:
    	/** our inherited run method */
    	virtual bool run();
    };

    class TestTask : public System::Runnable
    {
    public:
        TestTask(): counter(100000), finished(false){}
        
        virtual bool initial()
        {
            counter = 0;
            finished = false;
            return true;
        }
        
        virtual bool run()
        {
            for(uint32 i=0; i < 60; ++i)
            {
                System::Sleep(100);
                ++counter;
            }
            return false;
        }
        
        virtual void final()
        {
            finished = true;
        }
        
        int counter;
        bool finished;
    };


    /** Test our data types */
    class ThreadTest : public UnitTest
    {
    public:
    	ThreadTest() : UnitTest("Thread Logic"){}
    	virtual ~ThreadTest(){}
    	virtual void run_tests()
    	{
    	    RUN_TEST("Thread Basic Tests", basicThread());
    	    RUN_TEST("Mutex Tests", basicMutex());
    	    RUN_TEST("Semaphore Tests", basicSemaphore());
    	    RUN_TEST("Thread Cancel Tests", cancelThread());
    	    RUN_TEST("Thread Terminate Tests", terminateThread());
    	    RUN_TEST("Thread State Tests", stateCheckThread());
    	    RUN_TEST("Thread Pool Tests", managedThreads()); 
    	}
    private:
    	/** basic thread tests */
    	bool basicThread();
    	bool basic_thread_test(BasicThread *t1);
    	/** tests basic mutex locking */
    	bool basicMutex();
    	bool basic_mutex_test(MutexThread *t1);
    	/** test basic semaphore */
    	bool basicSemaphore();
    	/** test cancel thread (lets thread exit on its own) */
    	bool cancelThread();
    	/** test terminate thread (kill thread imiediatly */
    	bool terminateThread();
    	/** runs the thread in a while loop call doStateCheck then exits */
    	bool stateCheckThread();
    	/** test managed threads */
        bool managedThreads();
    };
   } /* Test */
    
} /* SRL */

#endif //__THREAD_TEST__
