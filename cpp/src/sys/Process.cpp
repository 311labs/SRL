#ifndef WIN32

#include "srl/sys/Process.h"
#include "srl/sys/System.h"
#include "srl/fs/File.h"
#include "srl/fs/FileStream.h"
#include <sys/wait.h>
#include <stdio.h>
using namespace SRL;
using namespace SRL::System;

#ifndef STDIN
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#endif

#define READ_PIPE 0
#define WRITE_PIPE 1

// FIXME need to look at arguments wrapped in quotes

const char delimiters[4] = " \"'";

bool get_delimiter(const char& c, char& delimiter)
{
    for(uint32 i=0; i<4; ++i)
    {
        if (c == delimiters[i])
        {
            delimiter = delimiters[i];
            return true;
        }
    }
    return false;
}


Process::Process(const String& command, uint32 uid, uint32 gid, bool mixin, int32 cpu_index) :
_pid(-1),
_user_info(),
_cpu_index(cpu_index),
_mixin(mixin),
_input(NULL), _output(NULL), _error(NULL),
_argv(16)
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
_input(NULL), _output(NULL), _error(NULL),
_argv(16)
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
_input(NULL), _output(NULL), _error(NULL),
_argv(16)
{
    _initialize(command);
    executeChild();
}



void Process::_initialize(const String& command)
{
    String arg(56);
    uint32 dpos = 0;
    _argc = 0;
    
    // first we get the file we are going to execute
    _file = command.section(' ', 0);
    // add the file to our argument list that we will pass to the exec call
    char *narg = new char[_file.length()+1];
    strncpy(narg, _file.text(), _file.length());
    narg[_file.length()] = '\0';
    _argv.add(narg);
    
    char delimiter = ' ';
    dpos = _file.length()-1;
    uint32 pos = dpos+1;
    while(pos<command.length())
    {
        // first get the delimiter
        if (get_delimiter(command[pos], delimiter))
        {
            arg.clear();
            if (delimiter == ' ')
            {
                if (get_delimiter(command[pos+1], delimiter))
                {
                    ++pos;
                }
            }
                
            //printf("new delimiter: '%c'\n", delimiter);
            ++pos;
        }
        
        // now lets get the value
        while ((pos < command.length()) && (command[pos] != delimiter) && (command[pos] != '\n'))
        {
            arg += command[pos];
            ++pos;
        }
        
        if (arg.size() && (arg != " "))
        {
            narg = new char[arg.length()+1];
            strncpy(narg, arg.text(), arg.length());
            narg[arg.length()] = '\0';
            _argv.add(narg);
            //printf("new arg: '%s'\n", arg.text());
        }
        
        if (command[pos] != ' ')
            ++pos;
    }

    _argv.add(NULL); // needed by the damn exec calls
}

Process::~Process()
{
    //printf("PROCESS TERMINATED\n");
    for(uint32 i=0; i<_argv.size(); ++i)
    {
        if (_argv[i] != NULL)
            delete[] _argv[i];
    }
    _argv.clear();
    
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
    terminate();
}

int Process::waitFor()
{
    int status;
    ::waitpid(_pid, &status, 0);
    _exit_value = status;
    return status;
}

bool Process::isRunning()
{
    int status;
    return (::waitpid(_pid, &status, WNOHANG) == 0);
}

void Process::terminate()
{
    int status;
    if (::waitpid(_pid, &status, WNOHANG) == 0)
    {
        // the process has not exited
        System::Kill(_pid, true);
        _exit_value = 1;
    }
    else
    {
        _exit_value = status;
    }
}

void Process::executeChild()
{
    int _stdin[2],_stdout[2],_stderr[2];
    // lets create our pipes
    if (::pipe(_stdin) | ::pipe(_stdout))
    {
        ::close(_stdin[0]);
        ::close(_stdin[1]);
        ::close(_stdout[0]);
        ::close(_stdout[1]);
        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
    }
    
    if (!_mixin)
    {
        ::pipe(_stderr);
    }
    
    // fork to our new process
    //printf("forking\n");
    pid_t pid = ::fork();
    //printf("forked\n");
    if (pid == -1)
    {
        // FORK FAILED
        printf("fork failed\n");
        ::close(_stdin[0]);
        ::close(_stdin[1]);
        ::close(_stdout[0]);
        ::close(_stdout[1]);
        if (!_mixin)
        {
            ::close(_stderr[0]);
            ::close(_stderr[1]);
        }
        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
    }
    else if (pid == 0)
    {
        //printf("fork succeeded!\n");
        // CHILD PROCESS
        // FIXME setsid would make this a daemon! Do we want this option?
        // close the childs stdin
        ::close(STDIN);
        // duplicate the childs stdin ti our stdin
        // NOTE dup returns the lowest available fd and remember (0=stdin,1=stdout,2=stderr)
        // READ_PIPE = 0
        // WRITE_PIPE = 1
        ::dup2(_stdin[READ_PIPE], STDIN);
        
        // clse the childs stdin for input and output
        // this child will now only be able to read from parent
        ::close(_stdin[READ_PIPE]);
        ::close(_stdin[WRITE_PIPE]);
        
        // close the childs stdout and make it go to our pipe
        ::close(STDOUT);
        ::dup2(_stdout[WRITE_PIPE], STDOUT);
        ::close(_stdout[READ_PIPE]);
        ::close(_stdout[WRITE_PIPE]);
              
        // close the childs stderr and make it go to our pipe
        ::close(STDERR);
        if (_mixin)
        {
            // duplicate the stdout.. this will make our fd=3 and be our stdout pipe too
            // THIS WORKS BUT OTHER THINGS SEEM TO GET MESSED UP!!!!
            ::dup2(STDOUT, STDERR);
        }
        else
        {
            ::dup2(_stderr[WRITE_PIPE], STDERR);            
            // now close the stderr pipe
            ::close(_stderr[READ_PIPE]);
            ::close(_stderr[WRITE_PIPE]);
        }
        
        // change the group of the process if specified
        if (_user_info.gid > 0)
        {
            if (::setgid(_user_info.gid))
            {
                // failed to set the user id
                throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
            }            
        }
        
        
        // change the owner of the process if specified
        if (_user_info.uid > 0)
        {
            if (::setuid(_user_info.uid))
            {
                // failed to set the user id
                throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
            }
            
            if (::seteuid(_user_info.uid))
            {
                // failed to set the user id
                throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
            }
        }
        
        // change the current working directory
        if (_user_info.home.length() > 0)
        {
            FS::SetCurrentDir(_user_info.home);
        }
        
        // check if we want process affinity
        if (_cpu_index >= 0)
        {
            // TODO: come back and fix the process affinity logic here
            //System::SetProcessAffinity((uint32)_cpu_index);
        }
        
        // execute our command
        //setvbuf(stdout, 0, _IONBF, 0);
        //setvbuf(stderr, 0, _IONBF, 0);
        ::execvp(_file.text(), _argv.asArray());
        printf("execvp failed '%s'\n", System::GetLastErrorString());
        ::exit(1);
    }
    else
    {
        // PARENT PROCESS
        _pid = pid;
        _input = new SRL::FS::FileStream(_stdin[WRITE_PIPE], "w");
        _output = new SRL::FS::FileStream(_stdout[READ_PIPE], "r");
        if (!_mixin)
        {
            _error =  new SRL::FS::FileStream(_stderr[READ_PIPE], "r");
        }
        else
        {
            _error = _output;
        }
        
        ::close(_stdin[READ_PIPE]);
        ::close(_stdout[WRITE_PIPE]);
        
        if (!_mixin)
        {    
            ::close(_stderr[WRITE_PIPE]);
        }
        
    }
    
}


#endif

