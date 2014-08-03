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

#ifndef __SRL_SYSTEM__
#define __SRL_SYSTEM__

#include "srl/String.h"

#if defined(linux) || defined(OSX)
    #include <pthread.h>
    #include <semaphore.h>
#else
    #define WIN32_LEAN_AND_MEAN
    #define _WIN32_WINNT 0x0501 
    #include "windows.h"

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#ifdef GetUserName
#undef GetUserName
#endif

#endif

// CPU INFO Status Flag
#define SINGLE_CORE_AND_HT_ENABLED 1
#define SINGLE_CORE_AND_HT_DISABLED 2
#define SINGLE_CORE_AND_HT_NOT_CAPABLE 4
#define MULTI_CORE_AND_HT_NOT_CAPABLE 5
#define MULTI_CORE_AND_HT_ENABLED 6
#define MULTI_CORE_AND_HT_DISABLED 7
#define USER_CONFIG_ISSUE 8


namespace SRL
{
    /** thread id can be a number or struct depending on the os */
    #if defined (WIN32)
        typedef uint64 THREAD_ID;
        #define NULL_THREAD 0
        /** returns the module instance */
        extern SRL_API HINSTANCE GetDllInstance();

    #else
        typedef pthread_t THREAD_ID;
        #define NULL_THREAD 0   
    #endif    
    
    namespace System
    {   

        /** returns a TimeValue for the given milliseconds */
        extern SRL_API TimeValue ConvertTimeValue(int milliseconds);
        /** returns the number of seconds and microseconds since the Epoch and the timezone (0-14) */
        extern SRL_API void GetCurrentTime(TimeValue &tv);
    
        /** returns the number of seconds since midnight as a floating point number (high precision) */
        extern SRL_API SRL::float64 GetNanoTimer();
        /** returns the number of seconds since the Epoch (low precision) */
        extern SRL_API SRL::float64 GetSystemTimer();

        /** returns the current machines hostname */
        extern SRL_API String GetHostName();
        /** returns the current username */
        extern SRL_API String GetUserName();
        /** Get Enviroment Variable Value */
        extern SRL_API String GetEnv(const String &env);
        /** Get Enviroment Variable Value */
        extern SRL_API void SetEnv(const String &env, const String &val);
        /** Get the last Error Number */
        extern SRL_API int GetLastErrorNumber();
        /** Get the Error String */
        extern SRL_API const char* GetErrorString(const int& errnum);
        /** Get the Error String */
        extern SRL_API const char* GetLastErrorString();        
        /** return the current process id */
        extern SRL_API uint64 GetPid(); 
        /** current thread will sleep for specified milliseconds */
        extern SRL_API void Sleep(const uint32& millis);
        /** reboot the current computer (requires proper administrative priviledges) */
        extern SRL_API void Reboot();
        
        #ifdef Yield
        #undef Yield
        #endif
        /** yield the calling thread to any other thread needing a processor time */
        extern SRL_API void Yield();
        
        /** represents a set of cpus used with process affinity routines */
        class SRL_API CpuSet
        {
        public:
            /** default constructor */
            CpuSet();
            /** add a cpu to the mask */
            void add(const uint32& cpu_index);
            /** remove a cpu from the mask */
            void remove(const uint32& cpu_index);
            /** tests if a cpu is in the mask */
            bool has(const uint32& cpu_index);
            /** remove all cpus from the mask */
            void clear();
            /** returns the maximum number of cpus the set can hold */
            uint32 maxSize();
#ifdef linux
            cpu_set_t affinity_mask;
#elif defined(WIN64)
            DWORD_PTR affinity_mask;
#elif defined(WIN32)
            DWORD affinity_mask;
#endif
        };
        
        /** get the current process affinity for the current process or a specified pid */
        extern SRL_API void GetProcessAffinity(CpuSet &affinity_mask, uint64 pid=0);
        /** specify the cpu mask a process can run on */
        extern SRL_API void SetProcessAffinity(CpuSet &affinity_mask, uint64 pid=0);
        
        /** access registers to extract information about the cpu */
        extern SRL_API void cpuid(int op, uint32 *eax, uint32 *ebx, uint32 *ecx, uint32 *edx);
        
        /** returns the number of current logical cpus */
        extern SRL_API uint32 GetCpuCount();
        
        /** get cpu counts */
        extern SRL_API bool GetCpuInfo(uint32 &physical_cpus, uint32 &logical_cpus, uint32 &cores_per_cpu, bool &ht_supported, bool &ht_enabled);
        
        
        /** Exit the current process with the given error code 
        * TODO: Logic for Exit 
        */
        extern SRL_API void Exit(int32 exitcode);


        /** run a command and return the output */
        extern SRL_API int RunEx(const String& command, String& output);
        /** run a command and return the output */
        extern SRL_API int RunAsEx(const String& command, const String& user, String& output);

        
        /** run a command and return the output */
        extern SRL_API String& Run(const String& command, String& output);
        /** run a command and return the output */
        extern SRL_API String& RunAs(const String& command, const String& user, String& output);
        
        struct UserInfo
        {
            UserInfo() : username(), password(), uid(0), gid(0), realname(), home(), shell(){}
            String username;
            String password;
            uint32 uid;
            uint32 gid;
            String realname;
            String home;
            String shell;
        };
        
        /** updates the user info struct for a given username */
        extern SRL_API void GetUserInfo(const String &user, UserInfo& user_info);
        
        /** return the uid for a user name */
        extern SRL_API uint32 GetUserID(const String &user);
        /** return the gid for a group name */
        extern SRL_API uint32 GetGroupID(const String &group);
                
        /** execute a command and return immediately (basically forks the process) */
        extern SRL_API uint64 Execute(const String& command);
        /** returns true if the pid is still running */
        extern SRL_API bool IsRunning(const uint64& pid);
        
        /** Kill a process by its pid */
        extern SRL_API bool Kill(const uint64& pid, bool force=false);
        
        /** returns the number of clock cycles since cpu reset */
        extern SRL_API uint64 CurrentCpuClocks();
        /** returns the number of clock cycles since cpu reset */
        extern SRL_API uint64 CurrentCpuFreq();

        /** Thread Priority Levels */
        enum THREAD_PRIORITY {
        #if defined (WIN32)
            THREAD_NORMAL=THREAD_PRIORITY_NORMAL, /**< normal priority(default) */
            THREAD_ABOVE_NORMAL=THREAD_PRIORITY_ABOVE_NORMAL,/**< Slightly above normal priority */
            THREAD_BELOW_NORMAL=THREAD_PRIORITY_BELOW_NORMAL,/**< Slightly below normal priority */
            THREAD_HIGHEST=THREAD_PRIORITY_HIGHEST,/**< Depending on the Parent Process priority level, this indicates a near realtime level */
            THREAD_IDLE=THREAD_PRIORITY_IDLE,/**< only executes when system is idle */
            THREAD_REALTIME=THREAD_PRIORITY_TIME_CRITICAL/**< caution!! will cause system wide slow down! not recommended */
        #else
            THREAD_IDLE=-2,/**< only executes when system is idle */
            THREAD_BELOW_NORMAL=-1,/**< Slightly below normal priority */
            THREAD_NORMAL=0, /**< normal priority(default) */
            THREAD_ABOVE_NORMAL,/**< Slightly above normal priority */
            THREAD_HIGHEST,/**< Depending on the Parent Process priority level, this indicates a near realtime level */
            THREAD_REALTIME/**< caution!! will cause system wide slow down! not recommended */
        #endif
        };
        /** get the calling threads priority level */
        extern SRL_API THREAD_PRIORITY GetCurrentThreadPriority();
            
        /** return the calling threads id */
        extern SRL_API THREAD_ID GetCurrentThreadID();
        /** compare two thread ids */
        extern SRL_API bool CompareThreadIDs(THREAD_ID id1, THREAD_ID id2);

    #ifdef WIN32
            // pointer to the NTDLL ZwQuerySystemInformation method
            static uint32 QuerySystemInfo(uint32 info_class, void* buffer, uint32 buffer_size, uint32* req_size);
    #endif

    }       
}

#endif // __SRL_SYSTEM__

