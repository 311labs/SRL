#ifdef WIN32

#include "srl/sys/Process.h"
#include "srl/sys/System.h"
#include "srl/fs/File.h"
#include "srl/fs/FileStream.h"

#include <io.h>
#include <fcntl.h>

using namespace SRL;
using namespace SRL::System;

Process::Process(const String& command, uint32 uid, uint32 gid, bool mixin, int32 cpu_index) :
_pid(-1),
_user_info(),
_cpu_index(cpu_index),
_mixin(mixin),
_input(NULL), _output(NULL), _error(NULL)
{
    _user_info.uid = uid;
    _user_info.gid = gid;
    _initialize(command);
    executeChild();
}

Process::Process(const String& command, const String& path, uint32 uid, uint32 gid, bool mixin, int32 cpu_index) :
_pid(-1),
_user_info(),
_cpu_index(cpu_index),
_mixin(mixin),
_input(NULL), _output(NULL), _error(NULL)
{
    _user_info.uid = uid;
    _user_info.gid = gid;
    _user_info.home = path;
    _initialize(command);
    executeChild();
}

Process::Process(const String& command, System::UserInfo& user_info, bool mixin, int32 cpu_index) :
_pid(-1),
_user_info(user_info),
_cpu_index(cpu_index),
_mixin(mixin),
_input(NULL), _output(NULL), _error(NULL)
{
    _initialize(command);
    executeChild();
}



void Process::_initialize(const String& command)
{
    _file = command;
}

Process::~Process()
{   
    ::CloseHandle(_hproc);
    
    if (_input != NULL)
    {
        _input->close();
        delete (SRL::FS::FileStream*)_input;
        _input = NULL;
    }

    if (_output != NULL)
    {
        _output->close();
        delete (SRL::FS::FileStream*)_output;
        _output = NULL;
    }

    if ((_mixin == false) && (_error != NULL))
    {
        _error->close();
        delete (SRL::FS::FileStream*)_error;
        _error = NULL;       
    }
  
}

int Process::waitFor()
{
    DWORD ret ;
    if ((ret = WaitForSingleObject(_hproc, INFINITE)) == WAIT_FAILED)
        return 0;

    // The process is terminated
    if (ret == WAIT_OBJECT_0)
    {
        // get the termination status
        DWORD ulExitCode = 0 ;
        if (GetExitCodeProcess(_hproc, &ulExitCode) == S_OK)
        {
            _exit_value = (int)ulExitCode;
            return _exit_value;
        }
    }
    return 0;
}

bool Process::isRunning()
{
    DWORD ret ;
    if ((ret = WaitForSingleObject(_hproc, 0)) == WAIT_FAILED)
        return true;

    if (ret == WAIT_OBJECT_0)
        return false;
    
    return true;
}

void Process::terminate()
{
    int status;
    if (isRunning())
    {
        // the process has not exited
        System::Kill(_pid, true);
        _exit_value = 1;
    }

}

/*
static HANDLE
find_file(char *exec_path, LPOFSTRUCT file_info)
{
	HANDLE exec_handle;
	char *fname;
	char *ext;

	fname = malloc(strlen(exec_path) + 5);
	strcpy(fname, exec_path);
	ext = fname + strlen(fname);

	strcpy(ext, ".exe");
	if ((exec_handle = (HANDLE)OpenFile(fname, file_info,
			OF_READ | OF_SHARE_COMPAT)) != (HANDLE)HFILE_ERROR) {
		free(fname);
		return(exec_handle);
	}

	strcpy(ext, ".cmd");
	if ((exec_handle = (HANDLE)OpenFile(fname, file_info,
			OF_READ | OF_SHARE_COMPAT)) != (HANDLE)HFILE_ERROR) {
		free(fname);
		return(exec_handle);
	}

	strcpy(ext, ".bat");
	if ((exec_handle = (HANDLE)OpenFile(fname, file_info,
			OF_READ | OF_SHARE_COMPAT)) != (HANDLE)HFILE_ERROR) {
		free(fname);
		return(exec_handle);
	}

	// should .com come before this case? 
	if ((exec_handle = (HANDLE)OpenFile(exec_path, file_info,
			OF_READ | OF_SHARE_COMPAT)) != (HANDLE)HFILE_ERROR) {
		free(fname);
		return(exec_handle);
	}

	strcpy(ext, ".com");
	if ((exec_handle = (HANDLE)OpenFile(fname, file_info,
			OF_READ | OF_SHARE_COMPAT)) != (HANDLE)HFILE_ERROR) {
		free(fname);
		return(exec_handle);
	}

	free(fname);
	return(exec_handle);
}
*/

/** 
 * In Win32, the first argument of CreateProcess() specifies the program
 * to run, and the second argument provides the command line arguments.
 * CreateProcess takes other process parameters as arguments. The
 * second-to-last argument is a pointer to a STARTUPINFORMATION
 * structure, which specifies the standard devices for the process and
 * other start up information about the process environment. You need
 * to set the hStdin, hStdout, and hStderr members of STARTUPINFORMATION
 * structure before passing its address to CreateProcess to redirect
 * standard input, standard output, and standard error of the process.
 * The last argument is a pointer to a PROCESSINFORMATION structure,
 * which is filled up by created processes. Once the process starts,
 * it will contain, among other things, the handle to the created
 * process.
 * 
 * http://www-128.ibm.com/developerworks/eserver/library/es-MigratingWin32toLinux.html
 * http://msdn2.microsoft.com/en-us/library/ms682499.aspx
 */
void create_child_pipe(HANDLE* pipe_rd, HANDLE* pipe_wr, SECURITY_ATTRIBUTES* sa)
{
    // create the pipe
    if (! ::CreatePipe(pipe_rd, pipe_wr, sa, 0))
        throw SRL::Errors::IOException(System::GetLastErrorString());
    // ensure the read handle to the stdout pipe is not inherited
    SetHandleInformation(*pipe_rd, HANDLE_FLAG_INHERIT, 0);    
}

void Process::executeChild()
{
    SECURITY_ATTRIBUTES sa;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    HANDLE stdin_pipes[2], stdout_pipes[2], stderr_pipes[2];
    HANDLE hstdout, hstdin, hstderr;

    // set the flag that allows our pipes to be inherited
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
	/** 
	* TODO
	 * We can't use NULL for lpSecurityDescriptor because that uses the
	 * default security descriptor of the calling process.  Instead we use
	 * a security descriptor with no DACL.  This allows nonrestricted
	 * access to the associated objects.
	 * 
	 * 
     sub_process *pproc;
 	SECURITY_ATTRIBUTES inherit;
 	BYTE sd[SECURITY_DESCRIPTOR_MIN_LENGTH];
 	pproc = malloc(sizeof(*pproc));
 	memset(pproc, 0, sizeof(*pproc));
 	if (!InitializeSecurityDescriptor((PSECURITY_DESCRIPTOR)(&sd),
					  SECURITY_DESCRIPTOR_REVISION)) {
		pproc->last_err = GetLastError();
		pproc->lerrno = E_SCALL;
		return((HANDLE)pproc);
	}

	inherit.nLength = sizeof(inherit);
	inherit.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)(&sd);
	inherit.bInheritHandle = TRUE; 
	 * 
	 */
    sa.lpSecurityDescriptor = NULL;
    
    // get the stdout handle
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);    
    // get the stderr handle
    hstderr = GetStdHandle(STD_ERROR_HANDLE);
    // get the stdin handle
    hstdin = GetStdHandle(STD_INPUT_HANDLE);
    
    // the parent gets pipe[0] and the child gets pipe[1]
    create_child_pipe(&stdin_pipes[1], &stdin_pipes[0], &sa);
    create_child_pipe(&stdout_pipes[0], &stdout_pipes[1], &sa);
    if (!_mixin)
        create_child_pipe(&stderr_pipes[0], &stderr_pipes[1], &sa);
    
    // setup the inherited input/outputs for our new process
    // TODO should this be set or added
    ::memset(&si, 0, sizeof(STARTUPINFO));
    GetStartupInfo(&si);
    si.dwFlags != STARTF_USESTDHANDLES;
    si.cb = sizeof(STARTUPINFO);
    si.hStdInput = stdin_pipes[1];
    si.hStdOutput = stdout_pipes[1];
    if (_mixin)
        si.hStdError = stdout_pipes[1];
    else
        si.hStdError = stderr_pipes[1];
    
    // create the process
    if ( ::CreateProcess(NULL, _file._strPtr(), NULL, NULL, 
                         TRUE, DETACHED_PROCESS, NULL, NULL, &si, &pi) == FALSE)
    {
        throw Errors::IOException(System::GetLastErrorString());
    }
    
    _pid = pi.dwProcessId;
    _hproc = pi.hProcess;
    // we don't need a handle to the main thread of the process
    ::CloseHandle(pi.hThread);
    
    // Close the halves of the pipes we are not going to use
    ::CloseHandle(stdin_pipes[1]);
    ::CloseHandle(stdout_pipes[1]);
    if (!_mixin)
        ::CloseHandle(stderr_pipes[1]);
    
    
    // turn the pipes into streams!!
    
    _input = new SRL::FS::FileStream(_open_osfhandle((long)stdin_pipes[0], _O_WRONLY | _O_APPEND), "w");
    ::CloseHandle(stdin_pipes[0]);
    _output = new SRL::FS::FileStream(_open_osfhandle((long)stdout_pipes[0], _O_RDONLY), "r");
    ::CloseHandle(stdout_pipes[0]);
    if (!_mixin)
    {
        _error =  new SRL::FS::FileStream(_open_osfhandle((long)stderr_pipes[0],_O_RDONLY), "r");
        ::CloseHandle(stderr_pipes[0]);
    }
}
#endif

