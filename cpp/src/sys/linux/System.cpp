#include "srl/sys/System.h"
#include "srl/Exceptions.h"

#include "srl/config.h"

#ifdef HAVE_SYS_TIME_H
	#include <sys/time.h>
#else
	#include <sys/timeb.h>
	#include <time.h>
#endif

#include <cstring>
#include <stdlib.h>

#include <sched.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/reboot.h>

#define MILLISECONDS_PER_SECOND 1000
#define NANOSECONDS_PER_SECOND 1000000000
#define NANOSECONDS_PER_MILLISECOND 1000000

using namespace SRL;

void System::GetCurrentTime(TimeValue &tv)
{
	timeval ctv;
	struct timezone tz;
	gettimeofday(&ctv, &tz);
	tv.tv_sec = ctv.tv_sec;
	tv.tv_usec = ctv.tv_usec;
    tzset();
	tv.tv_zone = timezone/3600;
    tv.tv_dst = daylight;
}

void System::Sleep(const uint32& millis)
{
	timespec ts;
	ts.tv_sec = (millis / MILLISECONDS_PER_SECOND);
	ts.tv_nsec = ((millis % MILLISECONDS_PER_SECOND) * NANOSECONDS_PER_MILLISECOND);
	
	if (ts.tv_nsec >= NANOSECONDS_PER_SECOND)
	{
		ts.tv_nsec -= NANOSECONDS_PER_SECOND;
		++ts.tv_sec;
	}

	::nanosleep(&ts, NULL);
}

void System::Reboot()
{
    sync();
    reboot(RB_AUTOBOOT);    
}

void System::Yield()
{
    ::sched_yield();
}

uint64 System::GetPid()
{
    return ::getpid();
}

String System::GetUserName()
{
    return ::getlogin();
}

String System::GetHostName()
{
	char hostname[128];
	int buf_size = 128;
    ::gethostname(hostname, buf_size);
	return hostname;
}

int System::GetLastErrorNumber()
{
    return errno;
}

uint64 System::CurrentCpuClocks()
{
     uint64 x;
     uint32 a, d;
     asm volatile ("rdtsc" : "=a" (a), "=d" (d));
     x = d;
     x = (x<<32)|a;
     return x;

}

bool System::Kill(const uint64& pid, bool force)
{
    for(uint32 i=0; i<10; ++i)
    {
        ::kill(pid, SIGTERM);
        if (!System::IsRunning(pid))
        {
            return true;
        }
        System::Sleep(500);
    }

    if (force)
    {
        for(uint32 i=0; i<10; ++i)
        {
            ::kill(pid, SIGKILL);
            if (!System::IsRunning(pid))
            {
                return true;
            }
            System::Sleep(500);
        }    
    }
    return false;
}

uint64 System::Execute(const String& command)
{
    String file = command.section(' ', 0);
    int argc = command.count(' ')+1;
    
    // number of arguments plus one null at the end
    
    char** argv = new char*[argc+1];
    argv[argc] = NULL;
    String arg;
    int slen = 0;
    for (int i=0; i< argc; ++i)
    {
        if (i != 0)
        {
            arg = command.section(' ', i);
        }
        else
        {
            // parse out the command name
        	if (file.count('\\') > 0)
        	{
        		arg = file.substr(file.rfind('\\')+1);	
        	}
        	else if (file.count('/') > 0)
        	{
        		arg = file.substr(file.rfind('/')+1);	
        	}
        	else
        	{
        	    arg = file;
        	}      
        }
        // add 1 for the null terminator
        slen = arg.length() + 1;
        argv[i] = new char[arg.length()+1];
        ::memcpy(argv[i], arg.text(), sizeof(char)*arg.length());
        argv[i][arg.length()] = '\0';
    }    
    
    // fork to our new process
    pid_t pid = ::fork();
    if (pid == -1)
    {
        throw Errors::IOException("::fork call failed");
    }
    else if (pid == 0)
    {
        // execute our command
        ::execvp(file.text(), argv);
        ::exit(1);
    }
    
    for (int i=0; i< argc; ++i)
    {
        delete[] argv[i];
    }
    delete[] argv;
    return pid;
    
}

bool System::IsRunning(const uint64 &pid)
{
    int status;
    return (::waitpid(pid, &status, WNOHANG) == 0);
}

bool System::CompareThreadIDs(THREAD_ID id1, THREAD_ID id2)
{
    return pthread_equal(id1, id2);

}

THREAD_ID System::GetCurrentThreadID()
{
	return pthread_self();
}

System::THREAD_PRIORITY System::GetCurrentThreadPriority()
{
	sched_param param;
	int policy;
	int ret;
	/* scheduling parameters of target thread */
	ret = pthread_getschedparam (System::GetCurrentThreadID(), &policy, &param);
	/* sched_priority contains the priority of the thread */
	return (THREAD_PRIORITY)param.sched_priority;
}


// This is linux only
#ifdef linux

System::CpuSet::CpuSet()
{
    clear();
}

void System::CpuSet::add(const uint32& cpu_index)
{
    CPU_SET(cpu_index, &affinity_mask);
}

void System::CpuSet::remove(const uint32& cpu_index)
{
    CPU_CLR(cpu_index, &affinity_mask);
}

bool System::CpuSet::has(const uint32& cpu_index)
{
    return CPU_ISSET(cpu_index, &affinity_mask);
}

uint32 System::CpuSet::maxSize()
{
    return CPU_SETSIZE;
}

void System::CpuSet::clear()
{
    CPU_ZERO(&affinity_mask);
}

void System::GetProcessAffinity(CpuSet &affinity_mask, uint64 pid)
{
	if (::sched_getaffinity(pid, sizeof(affinity_mask.affinity_mask), &(affinity_mask.affinity_mask)) < 0)
	{
	    throw Errors::IOException("::sched_getaffinity call failed");
	}
}

void System::SetProcessAffinity(CpuSet &affinity_mask, uint64 pid)
{
    sched_setaffinity(pid, sizeof(affinity_mask.affinity_mask), &(affinity_mask.affinity_mask));
}

uint32 System::GetCpuCount()
{
    return sysconf(_SC_NPROCESSORS_CONF);
}

#endif
