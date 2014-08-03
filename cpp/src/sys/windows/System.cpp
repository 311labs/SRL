#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <process.h>

#include <intrin.h>

#include <sys/timeb.h>
#include <time.h>

#include "srl/sys/System.h"
#include "srl/Exceptions.h"

using namespace SRL;

HINSTANCE _SRL_DLL_HINSTANCE = NULL;

HINSTANCE SRL::GetDllInstance()
{
    return _SRL_DLL_HINSTANCE;
}


BOOL WINAPI DllMain
(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved   // reserved
)
{
    switch( fdwReason )
    {
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:

        case DLL_PROCESS_ATTACH:
            _SRL_DLL_HINSTANCE = hinstDLL;
            
            break;
    }
    return TRUE;
}


void System::GetCurrentTime(TimeValue& tv)
{
    _tzset();
    __timeb64 timebuffer;

    _ftime64(&timebuffer);

    tv.tv_sec = timebuffer.time;
    tv.tv_usec = timebuffer.millitm*1000;
    tv.tv_zone = timebuffer.timezone/60;    
}

void System::Sleep(const uint32& millis)
{
    ::Sleep(millis);
}

void System::Reboot()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES  tkp;
    // Get a token for this process.
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

    // Get the LUID for the shutdown privilege.
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;  // one privilege to set
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    // Get the shutdown privilege for this process.
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

    // Reboot the system and force all applications to close.
    ExitWindowsEx(EWX_REBOOT, 0);  
}

void System::Yield()
{
    SleepEx(0, false);
}

uint64 System::GetPid()
{
    return ::_getpid();
}

String System::GetUserName()
{
    char username[128];
    DWORD buf_size = 128;
    ::GetUserNameA(username, &buf_size);
    return String(username);
}

String System::GetHostName()
{
    char hostname[128];
    int buf_size = 128;
    DWORD win_size = buf_size;
    ::GetComputerName(hostname, &win_size);
    return hostname;
}

int System::GetLastErrorNumber()
{
    return ::GetLastError();
}

uint64 System::CurrentCpuClocks()
{
    uint64 t;    
#ifdef WIN64
    // cool trick to preven optimization of call from www.agner.org
   int dummy[4];           // For unused returns 
   volatile int DontSkip;  // Volatile to prevent optimizing 
   __cpuid(dummy, 0);      // Serialize 
   DontSkip = dummy[0];    // Prevent optimizing away cpuid 
   t = __rdtsc();      // Read time 
   __cpuid(dummy, 0);      // Serialize again 
   DontSkip = dummy[0];    // Prevent optimizing away cpuid 
#else
    unsigned int a,b;
    unsigned int *c = (unsigned int *)&t;
    _asm {
    _emit 0x0f;
    _emit 0x31;
    mov a,eax;
    mov b,edx;
    }
    c[0]=a;c[1]=b;
#endif
    return t;
}

BOOL CALLBACK TerminateAppEnum( HWND hwnd, LPARAM pid )
{
    DWORD dwID ;
    ::GetWindowThreadProcessId(hwnd, &dwID) ;

    if(dwID == (DWORD)pid)
    {
        ::PostMessage(hwnd, WM_CLOSE, 0, 0) ;
    }
    return TRUE ;
}

bool System::Kill(const uint64& pid, bool force)
{
    HANDLE   proc_handle ;
    DWORD   dwRet ;
    
    // get the handle for the process with terminate rights
    proc_handle = ::OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE, pid);
    if (proc_handle == NULL)
    {
        // permission denied
        return false;
    }
    
    // EnumWindows calls our TerminateAppEnum function for the passed in pid
    ::EnumWindows((WNDENUMPROC)TerminateAppEnum,(LPARAM)pid);
    bool result = false;
    // wait for no more then 10 seconds to see if the process exits
    if (::WaitForSingleObject(proc_handle, 10000) != WAIT_OBJECT_0)
    {
        if (force)
        {
            result = ::TerminateProcess(proc_handle,0);
        }
    }
    ::CloseHandle(proc_handle);
    return result;
}

bool System::CompareThreadIDs(THREAD_ID id1, THREAD_ID id2)
{
    return (id1 == id2);
}

THREAD_ID System::GetCurrentThreadID()
{
    return ::GetCurrentThreadId();
}

System::THREAD_PRIORITY System::GetCurrentThreadPriority()
{
    return (THREAD_PRIORITY)::GetThreadPriority(GetCurrentThread());
}

bool System::IsRunning(const uint64 &pid)
{
    HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION, false, pid );
    if (hProcess == NULL)
        return false;
    DWORD exit_code;
    bool res = false;
    if (::GetExitCodeProcess(hProcess, &exit_code))
        if (exit_code == STILL_ACTIVE)
            res = true;
    ::CloseHandle(hProcess);
    return res;
}

uint64 System::Execute(const String& command)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD pid;
    // first we find the full path for the cmd
    String exe = command;
    /*String args;
	String exe_name;
	int pos = command.find(' ');
    if (pos > 0)
    {
        exe = command.left(pos);
		args = command.substr(pos);
    }
    
	pos = exe.rfind('\\');
	if (pos > 0)
	{
		exe_name = exe.right(exe.length()-pos);
	}
	else
	{
		exe_name = command;
	}
	args.prepend(exe_name);	*/

	::memset(&pi, 0, sizeof(pi));
    ::memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    //::GetStartupInfo(&si);
	
    // CreateProcess (exe_path_name, comamnd_line_args, )
    if (::CreateProcess(NULL, exe._strPtr(),
                        NULL, NULL, FALSE,
                        DETACHED_PROCESS | HIGH_PRIORITY_CLASS,
                        NULL, NULL, &si, &pi) == FALSE)
    {
        throw Errors::IOException(System::GetLastErrorString());
    }
    
    pid = pi.dwProcessId;
    CloseHandle(pi.hThread);
    
    return pid;
}

// PROCESS AFFINITY LOGIC

System::CpuSet::CpuSet()
{
    affinity_mask = 0;
}

void System::CpuSet::add(const uint32& cpu_index)
{
    // OR the cpu_index mask to our current affinity mask
    affinity_mask |= (1 << (cpu_index+1));
}

void System::CpuSet::remove(const uint32& cpu_index)
{
    // XOR the cpu_index mask to our current affinity mask
    affinity_mask ^= (1 << (cpu_index+1));
}

bool System::CpuSet::has(const uint32& cpu_index)
{
    return (affinity_mask & (1 << cpu_index));
}

uint32 System::CpuSet::maxSize()
{
    return sizeof(affinity_mask)*8;
}

void System::CpuSet::clear()
{
    affinity_mask = 0;
}

void System::GetProcessAffinity(CpuSet &affinity_mask, uint64 pid)
{
    HANDLE hProcess;
#ifdef WIN64
    DWORD systemAffinityMask;
#else
    DWORD_PTR systemAffinityMask;
#endif
    
    if (pid == 0)
        pid = ::GetCurrentProcessId();
        
    hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);    
    
    if (hProcess == NULL)
    {
        throw Errors::IOException("::OpenProcess call failed");
    }
    ::GetProcessAffinityMask(hProcess, &(affinity_mask.affinity_mask), &systemAffinityMask);
    ::CloseHandle(hProcess);
}

void System::SetProcessAffinity(CpuSet &affinity_mask, uint64 pid)
{
    HANDLE hProcess;    
    if (pid == 0)
        pid = ::GetCurrentProcessId();
        
    hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_SET_INFORMATION, FALSE, pid);    
    
    if (hProcess == NULL)
    {
        throw Errors::IOException("::OpenProcess call failed");
    }
    ::SetProcessAffinityMask(hProcess, affinity_mask.affinity_mask);
    ::CloseHandle(hProcess);
}

uint32 System::GetCpuCount()
{
    // now get the number of cpus
    SYSTEM_INFO sys_info;
    ::GetSystemInfo(&sys_info);
    return static_cast<uint32>(sys_info.dwNumberOfProcessors);   
}

#endif

