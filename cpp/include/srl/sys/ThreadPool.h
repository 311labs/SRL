#ifndef __SRLTHREADPOOL__
#define __SRLTHREADPOOL__

#include "srl/sys/Thread.h"
#include "srl/util/Vector.h"

namespace SRL
{
    namespace System
    {
        class Thread;
        class Runnable;
        
        /**
        * Abstract Base Class for Thread Managers
        */
        class SRL_API ThreadManager
        {
        friend class Thread;
        public:
            ThreadManager(){};
            virtual ~ThreadManager (){};

        protected:
            /** called by a thread when it has finished its task */
            virtual void task_completed(Thread* thread)=0;

        };
        
        /**
        * This is a very simple thread pool.
        * 
        */
        class SRL_API ThreadPool : protected ThreadManager
        {
        friend class Thread; 
        public:
            /**
            * Create a thread pool that will not grow past the max size
            * and will start with the min_size
            */
            ThreadPool(const uint32& max_size, uint32 min_size=2);
            virtual ~ThreadPool ();
            
            /** blocks until a thread becomes available then passes the task to the thread and returns */
            void runTask(Runnable* task);
            /** blocks until all tasks have been completed */
            void waitAll();
            /** returns the current size of the thread pool */
            const uint32& size(){ return _size; }
            /** returns the current number of available threads */
            uint32 availableCount(){ return _max_size - busyCount(); }
            /** returns the current number of busy threads */
            const uint32& busyCount(){ return _busy_pool.size(); }
            
            
        protected:
            uint32 _max_size, _min_size, _size;
            Util::Vector<Thread*> _available_pool;
            Util::Vector<Thread*> _busy_pool;
            Mutex _pool_lock;
            Semaphore _waiter, _blocker;
            Thread* _createThread();
            Thread* _getThread();
            /** called by a thread when it has finished its task */
            virtual void task_completed(Thread* thread);
        };
    } /* System */
    
} /* SRL */

#endif