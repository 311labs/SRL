#ifdef linux
#include "srl/sys/ProcessList.h"
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include "srl/fs/TextFile.h"
#include "srl/Console.h"

using namespace SRL;
using namespace SRL::System;

/**
* Process List Logic of Linux 
* Note CPU Usage Memory Usage require Root access
*/
void ProcessList::refresh()
{
    // only allow updates every 1 second at most
    float64 cur_time = GetSystemTimer();
    float64 delta_time = (cur_time - _last_update);
    
    if (delta_time < 1.0f)
        return;
        
    _last_update = cur_time;   
    
	DIR *parent;
	if ((parent = ::opendir("/proc")) == NULL)
	{
		throw Errors::IOException("call to opendir failed!");
	}

	// clear all existing pids
	_pids.clear();
	_new_pids.clear();

	dirent* child;
	Process *process = NULL;
	while (child = readdir( parent))
	{
		if (isdigit(child->d_name[0]))
		{
			uint32 pid = String::UInt(child->d_name);
			_pids.add(pid);
			// check if the pid exists
			process = getProcess(pid);
			// if the process does not exist create it
			if (process == NULL)
			{
				_new_pids.add(pid);
				// lets attempt to find the process 
				process = new Process(pid, _cpu_count);
			 	_list[pid] = process;
				if (_listener != NULL )
					_listener->event_process_started(*process);
			}
			else
				process->refresh();
		}
	}
	
	::closedir(parent);
	remove_dead();	
}


/** 
* Called when a process is first created
*/
void ProcessList::Process::initialize()
{
	String status_data;
	// catch any errors
	try
	{
		// dump the status file into a string
		SRL::FS::CatTextFile(String::Format("/proc/%d/status", _pid), &status_data);
		// command line
		SRL::FS::CatTextFile(String::Format("/proc/%d/cmdline", _pid), &_command);
		for (int l =0; l < _command.length(); ++l)
		{
			if (_command[l] == '\0')
			{
				_command[l] = ' ';
			}
		}
	}
	catch(...)
	{
		// assume the process exited
		_name = "<unknown>";
		return;
	}
	// filename
	_name = status_data.section('\n', 0).section('\t', 1);
	// parent_pid
	_parent_id = String::UInt(status_data.between("PPid:\t", '\n'));
	// user
	_uid = String::UInt(status_data.between("Uid:\t", '\t'));
	_owner = FS::GetUserFromUID(_uid);
	// group
	_gid = String::UInt(status_data.between("Gid:\t", '\t'));

	refresh();		   
}

void ProcessList::Process::refresh()
{
	static uint32 page_size = sysconf(_SC_PAGESIZE);
	String status_data, stat_data;
	uint32 tmp_user, tmp_kernel, tmp_created, vm_rss;
	char state;

	// lets update our process information
	try
	{
		// dump the status file into a string
		SRL::FS::CatTextFile(String::Format("/proc/%d/status", _pid), &status_data);
		// stat file
		// TODO look into doing a ScanTextFile here fscanf style
		SRL::FS::CatTextFile(String::Format("/proc/%d/stat", _pid), &stat_data);
		_is_running = true;
	}
	catch(...)
	{
		_is_running = false;
		return;
	}
	
	try
	{
	    DIR* parent = opendir(String::Format("/proc/%d/fd", _pid).text());
	    dirent* child;
	    _handles = 0;
	    if (parent != NULL)
	    {
	        while (child = readdir( parent))
	            ++_handles;
		}
		::closedir(parent);
	}
	catch(...)
	{
		_handles = 0;
	}
	
	// number of threads
	_threads = String::UInt(status_data.between("Threads:\t", '\n'));
	// update cpu information
	tmp_user = String::UInt(stat_data.section(' ', 13));
	tmp_kernel = String::UInt(stat_data.section(' ', 14));
	_cpu.update(tmp_user, tmp_kernel);
	// update memory
	_memory._virtual = String::UInt(stat_data.section(' ', 22));
	_memory._resident = ((String::UInt(stat_data.section(' ', 23))+3) * page_size);
	_memory._faults = String::UInt(stat_data.section(' ', 11));
}

void ProcessList::Process::cleanup()
{
    
}

#endif /* linux */

