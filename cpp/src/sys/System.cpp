
#include "srl/sys/System.h"
#include "srl/Exceptions.h"
#include "srl/sys/Process.h"
#ifdef WIN32
    #include <intrin.h>
	#include <process.h>
	#define popen _popen
	#define pclose _pclose
#else
    #include <pwd.h>
    #include <grp.h>
#endif
#include <stdio.h>

using namespace SRL;

TimeValue System::ConvertTimeValue(int milliseconds)
{
    TimeValue tv; tv.tv_sec = milliseconds/1000; tv.tv_usec = (milliseconds*1000)%1000000;
	return tv;
}

float64 System::GetNanoTimer()
{
	static uint64 cpu_freq = System::CurrentCpuFreq();
	return (float64)System::CurrentCpuClocks()/cpu_freq;
}

float64 System::GetSystemTimer()
{
	TimeValue now;
	float64 total_seconds = 0.0;
	System::GetCurrentTime(now);
	total_seconds = now.tv_sec;
	total_seconds += now.tv_usec / 1000000.0;
	return total_seconds;  
}

uint64 System::CurrentCpuFreq()
{
	// check clock speed
    uint64 last_clock = System::CurrentCpuClocks();
	System::Sleep(250);
    uint64 elapsed = System::CurrentCpuClocks();
    
	// get the number of ticks per second
    return (elapsed - last_clock) * 4;
}

String System::GetEnv(const String &env)
{
	return String(getenv(env.text()));
}

void System::SetEnv(const String &env, const String &value)
{
#ifdef WIN32
    putenv(String::Format("%s=%s", env.text(), value.text()).text());
#else
    setenv(env.text(), value.text(), 1);
#endif
}

const char* System::GetErrorString(const int& errnum)
{
    return ::strerror(errnum);
}

const char* System::GetLastErrorString()
{
    return ::strerror(System::GetLastErrorNumber());
}


void System::cpuid(int op, uint32 *eax_value, uint32 *ebx_value, uint32 *ecx_value, uint32 *edx_value)
{
#ifndef WIN32
    /** PIC on i386 uses a register to store the GOT (global offset table) address. This register is usually %ebx,
     * making it unavailable for use by inline assembly (and also restricting the compiler's register usage when
     * compiling C or C++ code).  To get around this i mov the contents of ebx into the esi register then do my call. 
     * Once the call is complete i exchange the ebx and esi data and my ebx data is now available in the esi register.
     * I have seen another way to do this by pushing the ebx register onto the stack and then pop it off when your are done.
    */
    __asm__(
        "mov %%ebx, %%esi\n\t"
        "cpuid\n\t"
        "xchgl %%ebx, %%esi"
        : "=a" (*eax_value),
          "=S" (*ebx_value),
          "=c" (*ecx_value),
          "=d" (*edx_value)
        : "a" (op)
        : "cc");
#elif defined(WIN64)
    int cpu_info[4];
    __cpuid(cpu_info, op);
    eax_value = cpu_info[0];
    ebx_value = cpu_info[1];
    ecx_value = cpu_info[2];
    edx_value = cpu_info[3];
#elif defined(WIN32)
    __asm
    {
		xor ecx, ecx;
        mov eax, op;
        cpuid;
        mov edi,[eax_value];
        mov esi,[ebx_value];
        mov[edi], eax;
        mov[esi], ebx;
        mov edi,[ecx_value];
        mov esi,[edx_value];
        mov[edi], ecx;
        mov[esi], edx;
	}
#endif
}


#ifndef WIN32
void System::GetUserInfo(const String &user, UserInfo& user_info)
{
    struct passwd *pw = getpwnam(user.text());
    if (pw == NULL)
    {
        printf("getpwnam returned NULL");
        throw SRL::Errors::IOException(System::GetLastErrorString());
    }
    
    user_info.username = pw->pw_name;
    user_info.password = pw->pw_passwd;
    user_info.uid = pw->pw_uid;
    user_info.gid = pw->pw_gid;
    user_info.realname = pw->pw_gecos;
    user_info.home = pw->pw_dir;
    user_info.shell = pw->pw_shell;
}

uint32 System::GetUserID(const String &user)
{
    struct passwd *pw = getpwnam(user.text());
    if (pw == NULL)
        throw SRL::Errors::IOException(System::GetLastErrorString());
    return pw->pw_uid;
}

uint32 System::GetGroupID(const String &name)
{
    struct group *grp = getgrnam(name.text());
    if (grp == NULL)
        throw SRL::Errors::IOException(System::GetLastErrorString());
    return grp->gr_gid; 
}

#else

void System::GetUserInfo(const String &user, UserInfo& user_info)
{    
    user_info.username = user;
}

#endif

// SYSTEM

int System::RunEx(const String& command, String& buffer)
{
    char line[128];
    FILE *fp = ::popen(command.text(), "r");
    do
    {   
        if (!::fgets( line, sizeof(line), fp))
            break;
        buffer += line;
    } while (true);

    return ::pclose(fp);
}

int System::RunAsEx(const String& command, const String& user, String& buffer)
{
    System::UserInfo user_info;
    System::GetUserInfo(user, user_info);
    System::Sleep(500);
    System::Process proc(command, user_info);
    String line(1024);
    while (!proc.output().atEnd())
    {        
        proc.output().readLine(line, false);
        //printf("stdout: %s\n", line.text());
        if (line.length() > 0)
            buffer += line;
        line.clear();
        
        if (proc.error().canRead(100))
        {
            proc.error().readLine(line, false);
            printf("stderr: %s\n", line.text());
            if (line.length() > 0)
                buffer += line;
            line.clear();            
        }
        else
        {
            //printf("nothing to read");
        }
    }

    return proc.waitFor();
}

String& System::Run(const String& command, String& buffer)
{
    char line[128];
    FILE *fp = ::popen(command.text(), "r");
    do
    {   
        if (!::fgets( line, sizeof(line), fp))
            break;
        buffer += line;
    } while (true);

    ::pclose(fp);
    return buffer;
}

String& System::RunAs(const String& command, const String& user, String& buffer)
{
    System::UserInfo user_info;
    System::GetUserInfo(user, user_info);
    System::Sleep(500);
    System::Process proc(command, user_info);
    String line(1024);
    while (!proc.output().atEnd())
    {        
        proc.output().readLine(line, false);
        //printf("stdout: %s\n", line.text());
        if (line.length() > 0)
            buffer += line;
        line.clear();
        
        if (proc.error().canRead(100))
        {
            proc.error().readLine(line, false);
            printf("stderr: %s\n", line.text());
            if (line.length() > 0)
                buffer += line;
            line.clear();            
        }
        else
        {
            //printf("nothing to read");
        }
    }

    return buffer;
}



