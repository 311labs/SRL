 #include "srl/sys/ProcessList.h"
#include "srl/sys/SystemInfo.h"

using namespace SRL;
using namespace SRL::System;

// ---- Begin Process List ----
ProcessList::ProcessList() : _listener(NULL)
{
	// get the number of cpus used to calculate cpu load
	_cpu_count = System::SystemInfo::GetInstance()->cpuCount();    
    refresh();
}

ProcessList::~ProcessList()
{
    Iterator iter = _list.begin();
    while (iter != _list.end())
    {
        delete iter.value();
        iter = _list.remove(iter);
    }
}

ProcessList::Process* ProcessList::getProcess(const uint32& pid) const
{
	Iterator iter = _list.find(pid);
	if (iter != _list.end())
		return iter.value();
	return NULL;    
}

ProcessList::Process* ProcessList::getProcess(const String& name, bool ignore_case) const
{
    if (!ignore_case)
    {
        Iterator iter = _list.begin();
        for (; iter != _list.end(); ++iter)
        {
            if (iter.value()->name() == name)
                return iter.value();
        }
    }
    else
    {
        String lowername = name;
        lowername.toLower();
        Iterator iter = _list.begin();
        for (; iter != _list.end(); ++iter)
        {
            String iname = iter.value()->name();
            iname.toLower();
            if (iname == lowername)
                return iter.value();
        }        
    }
    return NULL;
}

void ProcessList::remove_dead()
{
    // TODO: should i keep a deadlist or not.. i just don't know
    Iterator iter = _list.begin();
	_dead_pids.clear();
    while (iter != _list.end())
    {
        if (!_pids.containsValue(iter.key()))
        {
            if (_listener != NULL)
                _listener->event_process_stopped(*iter.value());
			_dead_pids.add(iter.value()->id());
            delete iter.value();
            iter = _list.remove(iter);
        }
        else
        {
            ++iter;
        }
    }
}

// ---- End Process List ----

// --- Begin Process ----
ProcessList::Process::Process(const uint32& pid, int cpu_count) :
_pid(pid),
_threads(0), _handles(0),
_memory(),
_cpu(cpu_count),
_name(), _owner(), _domain(),
_command(), _filename(), 
_created(),
_priority(0),
_uid(0), _gid(0),
_parent_id(0), _tty(0),
_is_running(false), _is_new(true)
{
	initialize();
	_is_new = false;
}

ProcessList::Process::~Process()
{
    cleanup();
}

bool ProcessList::Process::terminate(bool force)
{
	return System::Kill(_pid, force);
}

void ProcessList::Process::CPU::update(const uint32& user_time, const uint32& kernel_time)
{
	// update the timestamp
	float64 cur_timer = System::GetSystemTimer();
	// convert to jiffies (1/100) second
	uint32 diff_timer = (uint32)((cur_timer - _last_timer)*100.0);
	// set our last timer to the current timer
	_last_timer = cur_timer;
	// calculate the amount of jiffies since the last update
	uint32 diff_user = user_time - _user_time;
	uint32 diff_kernel = kernel_time - _kernel_time;
	// only update loads if this is not the first time
	if (!((_user_time == 0) && (_kernel_time == 0)))
	{
		if (diff_kernel > 0)
			_kernel_load = ((float32)diff_kernel/diff_timer)*100.0;
		else
			_kernel_load = 0.0f;

		if (diff_user > 0)
			_user_load = ((float32)diff_user/diff_timer)*100.0;
		else
			_user_load = 0.0f;
		
		uint32 total_time = diff_kernel+diff_user;
		if (total_time > 0)
		{
			_load = ((float32)total_time/diff_timer)*100.0f;
			_sys_load = _load / _cpu_count;
		}
		else
		{
			_load = 0.0f;
			_sys_load = 0.0f;
		}
	}
	
	// set our current time variables
	_kernel_time = kernel_time;
	_user_time = user_time;
}


// ----- End Process ----
