#include "srl/sys/Daemon.h"
#include <signal.h>
#include <sys/stat.h>
#include "srl/log/LogFile.h"
#include "srl/log/SysLog.h"

using namespace SRL;
using namespace SRL::System;

Daemon *Daemon::__instance = NULL;

void Daemon::__signal_handler(int sig)
{
    if (Daemon::__instance == NULL)
        return;
    switch(sig)
    {
        case SIGHUP:
            Daemon::__instance->reload_event(); 
            break;
        case SIGTERM:
            Daemon::__instance->stop_event(); 
            break;
        case SIGINT:
            Daemon::__instance->stop_event(); 
            break;
    }
    
}

Daemon::Daemon(const String& name) :
_log(Log::Logger::GetInstance(name)),
_name(name),
_path(),
_log_path(),
_user(),
_group(),
_keep_running(true)
{
    
}

Daemon::~Daemon()
{
    
}

bool Daemon::isRunning()
{
    return false;
}

void Daemon::install(const SRL::String& path)
{
    
}

void Daemon::uninstall()
{
    
}

bool Daemon::start()
{
    // this will fork the daemon into a new process
    _pid = fork();
    if (_pid < 0)
        return false;
    else if (_pid > 0)
        return true;
    
    // set the file mode mask to we have permissions to write files
    umask(0);
    
    // become the process group/session leader
    Log::Logger::SetOutput(new SRL::Log::SysLog(_name));
    pid_t sid = setsid();
    if (sid < 0) 
    {
        _log.error("setsid failed");
        /* Log any failure here */
        exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    

    runForever();
    return true;
}

void Daemon::_initialize()
{
    _log.info("service is now initializing");
    // get our pid
    _pid = getpid();
    
    // check if we need to change the working directory
    if (!_path.isEmpty())
    {
        FS::SetCurrentDir(_path);
    }

    // check if we need to change the current group
    if (!_group.isEmpty())
    {
        // TODO impelement service group override
    }

    // check if we need to change the current user
    if (!_user.isEmpty())
    {
        // TODO impelement service user override
    }

    // check were to log to
    if (_log_path.isEmpty())
    {
        // we are in daemon mode so default to syslog
        // FIXME what to do if not in daemon mode
        //Log::Logger::SetOutput(new SRL::Log::SysLog(_name));
    }
    else
    {
        String log_path = String::Format("%s/%s.log", _log_path.text(), _name.text());
        Log::Logger::SetOutput(new SRL::Log::LogFile(log_path));
    }
    
    Daemon::__instance = this;
    
    // catch SIGHUP, SIGTERM and SIGINT
    signal(SIGHUP,Daemon::__signal_handler);
    signal(SIGTERM,Daemon::__signal_handler);
    signal(SIGINT,Daemon::__signal_handler);
    
    _log.info("service is now running");
}

void Daemon::runForever()
{
    try
    {
        _initialize();
        if (initial())
        {
            while (_keep_running && run())
            {
                System::Sleep(500);
            }
        
            final();
        }
    }
    catch (SRL::Errors::Exception &e)
    {
        _log.error(e);
    }

}

bool Daemon::stop()
{
    _keep_running = false;
    return false;
}

bool Daemon::reload()
{
    return false;
}

