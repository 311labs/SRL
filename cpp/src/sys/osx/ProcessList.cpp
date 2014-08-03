#include "srl/sys/ProcessList.h"
#ifdef OSX

#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <mach/mach.h>
#include <mach-o/arch.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

using namespace SRL;
using namespace SRL::System;

static mach_port_t srl_port = 0;
#define SECONDS_TO_JIFFIES(a) (a * 32768.0)
#define TIME_VALUE_TO_JIFFIES(a) ((a).seconds * 100) + ((a).microseconds / 10000) 

/**
* Process List Logic of OSX (10.4)
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
    
    // let ask the kernel for all the processes
    struct kinfo_proc *kprocbuf;
    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
    size_t buffer_size = 0;
    // first we figure out how much memory we will need
    if (::sysctl(mib, 4, NULL, &buffer_size, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed! (initial size request)");

    kprocbuf = (struct kinfo_proc*)malloc(buffer_size);
    // TODO: add retry     
    if (::sysctl(mib, 4, kprocbuf, &buffer_size, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed! (process list request)");
    
    int proc_count = buffer_size/sizeof(struct kinfo_proc);
    // clear all existing pids
    _pids.clear();
    _new_pids.clear();



    if (srl_port == 0)
    {
        srl_port = mach_host_self();
    }

    // generate the basic process list
    Process *process = NULL;   
    for (int i=0; i < proc_count; ++i)
    {
        uint32 pid = kprocbuf[i].kp_proc.p_pid;
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
    free(kprocbuf);
    remove_dead();
}


/** 
* Called when a process is first created
*/
void ProcessList::Process::initialize()
{
    _mac_args = NULL;
    static int argmax = 0;
    // retrieve the basics like name, parent_pid, _uid, _gid, owner
    struct kinfo_proc p;
    
    int mib[4];
    
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = _pid;
    
    size_t len = sizeof(struct kinfo_proc);
    
    if (::sysctl(mib, 4, &p, &len, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for KERN_PROC_PID!");
        
    _parent_id = p.kp_eproc.e_ppid;
    _uid = p.kp_eproc.e_ucred.cr_uid;
    _owner = FS::GetUserFromUID(_uid);
    _gid = p.kp_eproc.e_pgid;
    if (p.kp_proc.p_comm != NULL)
        _name = p.kp_proc.p_comm;
    else
        _name = "<unknown>";
    
    
    // get the command line arguments
    /* Get the maximum process arguments size. */
    if (_mac_args == NULL)
    {
        mib[0] = CTL_KERN;
        mib[1] = KERN_ARGMAX;
        mib[2] = 0;
        len = sizeof(argmax);
        if (sysctl(mib, 2, &argmax, &len, NULL, 0) != -1)
        {
            if (argmax > 8192)
                argmax = 8192;
            _mac_args = (char *)malloc(argmax);
        }
    }

    mib[0] = CTL_KERN;
    mib[1] = KERN_PROCARGS;
    mib[2] = _pid;
    mib[3] = 0;
    
    char *cp;
    len = (size_t)argmax;

    if (::sysctl(mib, 3, _mac_args, &len, NULL, 0) != -1)
    {
        // skip the saved exec path, by finding the first null char
        for (cp = _mac_args; cp < &_mac_args[len]; ++cp)
            if (*cp == '\0')
                break;
        
        // now move to the first arg position by finding the first none null char
        for (;cp < &_mac_args[len]; ++cp)
            if (*cp != '\0')
                break;            
        
        _filename = cp;        
        _command = cp;
        // now copy each string into our argument list
        for (;cp < &_mac_args[len]; ++cp)
        {
            if (*cp == '\0')
            {
                while (*cp == '\0')
                    ++cp;
                _command += " ";
                _command += cp;          
            }
        }
    }

    refresh();    
}

void ProcessList::Process::refresh()
{
    struct kinfo_proc p;
    task_t task;
    
    int mib[4];
    
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = _pid;
    
    size_t len = sizeof(struct kinfo_proc);
    
    if (::sysctl(mib, 4, &p, &len, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for KERN_PROC_PID!");    
    
    _priority = p.kp_proc.p_priority;
    
    if (task_for_pid(mach_task_self(), _pid, &task) != KERN_SUCCESS)
    {
        // most likely not priviledge account (Non Root)
        return;
    }
    
    task_basic_info_data_t info;
    mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;    
    if(task_info(task, TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS) 
    {
        thread_array_t  thread_table;
        thread_basic_info_t thi;
        thread_basic_info_data_t thi_data;
        //kern_return_t error;  
        //int pid;    
        uint32 cnt, table_size;        
        
        _memory._resident = info.resident_size;// 1024;
        _memory._virtual = info.virtual_size;// 1024;
        // TODO Adjust Virtual to not include globally shared text and data regions

        // thread count
        if (task_threads(task, &thread_table, &table_size) != KERN_SUCCESS)
        {
            // most likely not priviledge account (Non Root)
            return;
        }            

        _threads = table_size;
        uint32 user_time = 0;
        uint32 sys_time = 0;
        thi = &thi_data;
        user_time = TIME_VALUE_TO_JIFFIES(info.user_time);
        sys_time = TIME_VALUE_TO_JIFFIES(info.system_time);

        for (uint32 t = 0; t < table_size; t++) 
        {
            cnt = THREAD_BASIC_INFO_COUNT;
            if (thread_info(thread_table[t], THREAD_BASIC_INFO,(thread_info_t)thi, &cnt) != KERN_SUCCESS) 
                continue;
        
            if ((thi->flags & TH_FLAGS_IDLE) == 0)
            {
                user_time += TIME_VALUE_TO_JIFFIES(thi->user_time);
                sys_time += TIME_VALUE_TO_JIFFIES(thi->system_time);
            }
        }

        _cpu.update(user_time, sys_time);
        // cleanup thread table
        vm_deallocate(mach_task_self(), (vm_offset_t)thread_table,
            table_size * sizeof(thread_array_t));      

        task_events_info_data_t events;
        cnt = TASK_EVENTS_INFO;
        if(task_info(task, TASK_EVENTS_INFO, (task_info_t)&events, &cnt) == KERN_SUCCESS) 
        {
            _memory._faults = events.faults;
            _memory._pageins = events.pageins;
        }
      
    }

}

void ProcessList::Process::cleanup()
{
    if (_mac_args != NULL)
        free(_mac_args);
}


#endif /* OSX */

