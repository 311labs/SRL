#ifdef WIN32
#include "srl/sys/ProcessList.h"
#include "srl/sys/SystemQuery.h"

//#include "srl/Console.h"

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
    
	//Console::writeLine("ENTER: ProcessList.refresh()");
	float system_load = 0;
	//. get the page size
	static uint32 page_size = 0;
	if (page_size == 0)
	{
		SYSTEM_INFO s;
        	GetSystemInfo(&s);
		page_size = ((uint32)s.dwPageSize);
	}

	// Create a Buffer to hold our data
	SystemQuery query(QUERY_PROCESS_INFO);
	// pointer to the first item in the proc info list
	SYSTEM_PROCESS_INFORMATION* process_info =
			(SYSTEM_PROCESS_INFORMATION*)query.buffer;

	// clear all existing pids
	_pids.clear();
	_new_pids.clear();

	_pids.add(0);

	Process* process = NULL;
	do
	{
		
		if (process_info->id > 0)
		{
			bool is_new_proc = false;
			// add the pid to the pids vector (used to clear out dead processes)
			_pids.add(process_info->id);
		
			// check if the process exists
			process = getProcess(process_info->id);
			// if the process does not exist create one
			// lets ignore the System Idle process
			if (process == NULL)
			{
				is_new_proc = true;
				_new_pids.add(process_info->id);
				process = new Process(process_info->id, _cpu_count);
				process->_parent_id = process_info->parent_id;

				if (process_info->name.length > 0)
				{
					uint32 str_size = process_info->name.length /2;
					process->_name.resize(str_size);
					::WideCharToMultiByte(CP_ACP, 0, process_info->name.buffer, -1,
						process->_name._strPtr(), str_size, NULL, NULL);
					// process->_name = process->_name.toLower();
					// time the process was created
					process->_created = DateTime(process_info->create_time);

				}
				// check for system processes
				if ((process->id() == 4)||(process->id() == 8))
				{
					process->_name = "[System]";
				}
				else if (process_info->name.length == 0)
				{
					process->_name = "<unknown>";
				}

				// last add our new process to the list
				_list[process->id()] = process;
			}
			// update the process with the latest process info
			process->_cpu.update(
				FILETIME_TO_JIFFIES(process_info->user_time),
				FILETIME_TO_JIFFIES(process_info->kernel_time));
			// update system idle
			system_load += process->_cpu.load();

			// basic info
			process->_handles = process_info->handle_count;
			process->_threads = process_info->thread_count;
			process->_memory._resident = process_info->vm_counters.working_set_size;
			process->_memory._virtual = process_info->vm_counters.pagefile_usage;
			//process->_memory._resident = static_cast<uint32>(process_info->vm_counters.working_set_size / page_size);
			process->_memory._faults = process_info->vm_counters.page_fault_count;

			if ((is_new_proc) && (_listener != NULL ))
				_listener->event_process_started(*process);
		}
		// check if we have more processes
		if (process_info->next_entry_delta == 0)
			break;
			
		// iterate our pointer to the next process_info
		process_info = (SYSTEM_PROCESS_INFORMATION*)(((byte*)process_info)+(process_info->next_entry_delta));
		
	} while (true);
	
	Process *idle_proc;
	if (_list.hasKey(0))
	{
		idle_proc = _list[0];
	}
	else
	{
		// lets add the idle process too
		idle_proc = new Process(0, _cpu_count);
		idle_proc->_name = "[System Idle]";
		idle_proc->_owner = "SYSTEM";
		_list[0] = idle_proc;
		_new_pids.add(0);
	}
	idle_proc->_cpu._sys_load = 100.0f - system_load;

	remove_dead();
	//Console::writeLine("EXIT: ProcessList.refresh()");
}


/** 
* Called when a process is first created
*/
void ProcessList::Process::initialize()
{
	bool status = true;
	bool cleanup = false;
	// open a handle to the process
 	HANDLE proc_handle = ::OpenProcess( PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ, FALSE, _pid );
	
	if (proc_handle != NULL)
	{
		HANDLE token_handle = NULL;
 		if(::OpenProcessToken(proc_handle, TOKEN_READ, &token_handle))
 		{
 			DWORD token_length = sizeof(TOKEN_USER);
 			//PTOKEN_OWNER token_info = new TOKEN_OWNER;
			TOKEN_USER *ptoken_user, token_user;

 			if (!::GetTokenInformation(token_handle, TokenUser, &token_user, token_length, &token_length))
 			{
				ptoken_user = (TOKEN_USER*)malloc(token_length);
				cleanup = true;
				if (!::GetTokenInformation(token_handle, TokenUser, ptoken_user, token_length, &token_length))
				{
					status = false;
				}
			}
			else
			{
				cleanup = true;
				ptoken_user = &token_user;
			}

			if (status)
			{
 				SID_NAME_USE snu;
 				char user[MAX_PATH];
 				char domain[MAX_PATH];
 				uint32 usize = MAX_PATH;
 				uint32 dsize = MAX_PATH;
 
	 			if (::LookupAccountSid(NULL, ptoken_user->User.Sid, user, (LPDWORD)&usize, domain, (LPDWORD)&dsize, &snu))
 				{
					_owner = user;
					String sdomain = domain;
					if (strlen(domain) > 0)
						_owner = sdomain + "/" + _owner;
				}
				else
				{
					status = false;
				}
 			}
			if (cleanup)
				free(ptoken_user);
 			::CloseHandle(token_handle);
 		}
		::CloseHandle(proc_handle);
	}

	if (!status)
	{
		_owner = "unknown";
	}   
}

void ProcessList::Process::refresh()
{

}

void ProcessList::Process::cleanup()
{
    
}

#endif /* WIN32 */

