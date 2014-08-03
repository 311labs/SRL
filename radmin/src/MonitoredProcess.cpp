#include "srl/radmin/MonitoredProcess.h"
#include "srl/fs/TextFile.h"
#include "srl/sys/System.h"

using namespace SRL;
using namespace SRL::Radmin;


// FIXME my using the command name as the monitor name we can only have 1 monitored process for a single command
// funning at a time
MonitoredProcess::MonitoredProcess(const String& cmd, System::UserInfo &user_info) :
Monitor(cmd.section(' ', 0), "MonitoredProcess"),
_cmd(cmd),
_log_path(),
_user(user_info),
_process(NULL),
_pid(0)
{
    setSleep(500);
}

MonitoredProcess::MonitoredProcess(const String& cmd) :
Monitor(cmd.section(' ', 0), "MonitoredProcess"),
_cmd(cmd),
_log_path(),
_user(),
_process(NULL),
_pid(0)
{
    setSleep(500);
}

void MonitoredProcess::setLogPath(const String& log_path)
{
    _log_path = log_path;
}

MonitoredProcess::~MonitoredProcess()
{
    if (_process != NULL)
        delete _process;
    _process = NULL;
}

bool MonitoredProcess::initial()
{
    // create the process here
    //printf("process(%s, %d)", _cmd.text(), _user.uid);
    if (_user.uid != 0)
        _process = new System::Process(_cmd, _user);
    else
        _process = new System::Process(_cmd);
    _pid = _process->pid();
    //printf("process created(%d)\n", _pid);
    return true;
}

bool MonitoredProcess::run()
{
    String line(1024);
    FS::TextFile *log_file = NULL;
    //printf("running process\n");
    if (!_log_path.isEmpty())
    {
        //printf("creating log file: %s (%d)\n", _log_path.text(), _user.uid);
        log_file = FS::CreateTextFile(_log_path, true);
        if (_user.uid != 0)
            FS::ChangeOwner(_log_path, _user.uid, _user.gid);
    }
    //printf("entering loop\n");
    
    while (!_process->output().atEnd())
    {
        //printf("reading output from process\n");
        //
        _process->output().readLine(line, false);
        //printf(line.text());
        // log to file if specified
        if (log_file != NULL)
        {
            log_file->write(line);
            log_file->commit();
        }
        
        if (!line.isEmpty())
        {
            //printf("out: %s\n", line.text());
            if (!checkKeywords(line))
                return false;
        }
        doStateCheck(); // checks the state of the thread (for cancles, etc)
//        else
//            printf("empty line");
    }
    
    if (log_file != NULL)
        log_file->close();
    return false;    
}

void MonitoredProcess::final()
{
    if (_process != NULL)
        delete _process;
    _process = NULL;
    Monitor::final();
}

void  MonitoredProcess::thread_exception(SRL::Errors::Exception &e)
{
    printf("error: %s\n", e.message().text());
}

RPC::RpcStructValue* MonitoredProcess::asRpcValue() const
{
    RPC::RpcStructValue *value = Monitor::asRpcValue();
    value->set("pid", _pid);
    if (_process == NULL)
        value->set("is_running", false);
    else if (_process->isRunning())
        value->set("is_running", true);
    else
        value->set("is_running", false);
    
    return value;
}

