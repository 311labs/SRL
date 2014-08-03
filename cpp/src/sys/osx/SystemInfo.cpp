#include "srl/sys/SystemInfo.h"
#include "srl/sys/CpuInfo.h"

#ifdef OSX

#include "srl/fs/Directory.h"

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

host_t __mach_host;

// ------------ Class System CPU ----------------
void SystemInfo::CPU::update_times()
{
    uint32 processorCount = 0;
    processor_cpu_load_info_t processorTickInfo;
    mach_msg_type_number_t processorMsgCount;    
    
    if (::host_processor_info(__mach_host, PROCESSOR_CPU_LOAD_INFO, &processorCount, 
    		(processor_info_array_t *)&processorTickInfo, &processorMsgCount) != KERN_SUCCESS)
    {
        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
    }

	uint32 new_user = 0;
	uint32 new_kernel = 0;
	uint32 new_idle = 0;
	uint32 new_nice = 0;    
    
    if (!_is_avg)
    {
    	new_user = processorTickInfo[_number].cpu_ticks[CPU_STATE_USER];
    	new_kernel = processorTickInfo[_number].cpu_ticks[CPU_STATE_SYSTEM];
    	new_idle = processorTickInfo[_number].cpu_ticks[CPU_STATE_NICE];
    	new_nice = processorTickInfo[_number].cpu_ticks[CPU_STATE_IDLE];
	}
	else
	{
	    for( uint32 i = 0; i < processorCount; ++i )
	    {
        	new_user += processorTickInfo[i].cpu_ticks[CPU_STATE_USER];
        	new_kernel += processorTickInfo[i].cpu_ticks[CPU_STATE_SYSTEM];
        	new_idle += processorTickInfo[i].cpu_ticks[CPU_STATE_NICE];
        	new_nice += processorTickInfo[i].cpu_ticks[CPU_STATE_IDLE];	       
	    }
	}
	vm_deallocate(mach_task_self(), (vm_address_t) processorTickInfo, processorCount * sizeof(int));
	
	if (_last_update != 0.0f)
	{
		uint32 delta_user = new_user - _user;
		uint32 delta_kernel = new_kernel - _kernel;
		uint32 delta_nice = new_nice - _nice;
		uint32 delta_idle = new_idle - _idle;
	
		uint32 load_total = delta_user + delta_kernel;
		uint32 cpu_total  = load_total + delta_idle + delta_nice;
		_load = 100.0f * (float32)load_total/(float32)cpu_total;
		_user_load = 100.0f * ((float32)delta_user)/(float32)cpu_total;
		//_user_load = 100.0f * (float32)(delta_user+delta_nice)/(float32)cpu_total;
		_kernel_load = 100.0f * (float32)delta_kernel/(float32)cpu_total;
	}

	_user = new_user;
	_kernel = new_kernel;
	_nice = new_nice;
	_idle = new_idle;    

}

void SystemInfo::_update_os()
{
    int mib[4];
    char buf[256];
    size_t buf_size = 256;
  
    // this is used for mach calls later on
    __mach_host = mach_host_self();
  
    // lets get the machine type
    mib[0] = CTL_HW;
    mib[1] = HW_MACHINE;
    if (::sysctl(mib, 2, &buf, &buf_size, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for HW_MACHINE!");
    buf[buf_size] = '\0';
    _os._machine = buf;
  
    // lets get the kernel type
    buf_size = 256;
    mib[0] = CTL_KERN;
    mib[1] = KERN_OSTYPE;
    if (::sysctl(mib, 2, &buf, &buf_size, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for KERN_OSTYPE!");    
    buf[buf_size] = '\0';
    _os._kernel = buf;
    String tmp; 
    System::Run("/usr/bin/sw_vers", tmp);
    if (tmp.count('\n') > 1)
    {
        String distro = tmp.section('\n', 0);
        distro = distro.substr(distro.find(':')+2);
        _os._name = distro;
        distro = tmp.section('\n', 1);
        distro = distro.substr(distro.find(':')+2);
        _os._name += " " + distro;
        distro = tmp.section('\n', 2);
        distro = distro.substr(distro.find(':')+2);
        _os._build = distro;
    }
    else
        _os._name = "OS X";
    // lets get the kernel version
    buf_size = 256;
    mib[0] = CTL_KERN;
    mib[1] = KERN_OSRELEASE;
    if (::sysctl(mib, 2, &buf, &buf_size, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for KERN_PROC_PID!");    
    buf[buf_size] = '\0';
    _os._version = buf;

    // lets get the host and domain name
    buf_size = 256;
    mib[0] = CTL_KERN;
    mib[1] = KERN_HOSTNAME;
    if (::sysctl(mib, 2, &buf, &buf_size, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for KERN_PROC_PID!");    
    buf[buf_size] = '\0';
	tmp = buf;
	_host_name = tmp.section('.', 0);
	_domain = tmp.substr(tmp.find('.'));
	
	 
}

void SystemInfo::_update_cpus()
{
    int mib[4];  
    
    // lets get the cpu count
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    int cpu_count = 0;
    size_t len = sizeof(cpu_count);
    if (::sysctl(mib, 2, &cpu_count, &len, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for HW_NCPU!");
	
    mib[0] = CTL_HW;
    mib[1] = HW_CPU_FREQ;
    uint32 freq = 0;
    len = sizeof(freq);
    if (::sysctl(mib, 2, &freq, &len, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for HW_CPU_FREQ!");	

	double dfreq = freq/1000000000.0;

    const NXArchInfo *archinfo = ::NXGetLocalArchInfo();
    
	for (int c=0; c<cpu_count;++c)
	{
		CPU *cpu = new CPU(c);
		_cpus.add(cpu);
		cpu->_freq = dfreq;
		cpu->_vendor_id = archinfo->name;
		cpu->_model_name = archinfo->description;
	}
	
	
	// now for the cpu info
    _update_cpu_info();
    
    if (CpuInfo::IsIntel())
    {
        String vendorId = CpuInfo::GetVendorId();
        String model = CpuInfo::GetModelName();
        uint32 ifamily, imodel, itype, eax, unused;
        
        cpuid(1,&eax,&unused,&unused,&unused);
        imodel = (eax >> 4) & 0xf;
        ifamily = (eax >> 8) & 0xf;
        itype = (eax >> 12) & 0x3;        
        
    	for (int c=0; c<cpu_count;++c)
    	{
    		_cpus[c]->_model_name = model;
    		_cpus[c]->_vendor_id = vendorId;
    		_cpus[c]->_family = ifamily;
    		_cpus[c]->_model = imodel;
    		_cpus[c]->_type = itype;
    	} 
    }

	// cpu averager
	if (_cpus.size() > 1)
	{
		_cpu_avg  = new CPU(0, true);
		_cpu_avg->_vendor_id = _cpus[1]->_vendor_id;
		_cpu_avg->_model_name = _cpus[1]->_model_name;
		_cpu_avg->_model = _cpus[1]->_model;
		_cpu_avg->_family = _cpus[1]->_family;
		_cpu_avg->_type = _cpus[1]->_type;
		_cpu_avg->_freq = _cpus[1]->_freq;
		_cpu_avg->_cache = _cpus[1]->_cache;
	}
	else
		_cpu_avg = _cpus[0];

	
	struct loadavg cpu_times;
    mib[0] = CTL_VM;
    mib[1] = VM_LOADAVG;

    len = sizeof(cpu_times);
    if (::sysctl(mib, 2, &cpu_times, &len, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for VM_LOADAVG!");   
    
        
    // we will update the memory totals here too
    mib[0] = CTL_HW;
    mib[1] = HW_PHYSMEM;
    int mem_size = 0;
    len = sizeof(mem_size);
    if (::sysctl(mib, 2, &mem_size, &len, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for HW_PHYSMEM!");   
    _memory->_total = mem_size;

    // lets assume the swap file is /private/var/vm/swapfile
    if (FS::Exists("/private/var/vm"))
    {
        _swap->_total = 0;
        FS::FsObject *fsobj;
        FS::Directory swapdir("/private/var/vm");
        for (uint32 d=0; d<swapdir.totalChildren();++d)
        {
            fsobj = swapdir.getChild(d);
            if (fsobj->name().count("swapfile") == 1)
            {
                if (fsobj->size() > 0)
                {
                    _swap->_total += (uint32)fsobj->size();
                }
            }
        }
    }
    
}

void SystemInfo::_update_memory()
{
    vm_statistics_data_t vmStat;
    uint32 vmCount = HOST_VM_INFO_COUNT;
    
    if (::host_statistics(__mach_host, HOST_VM_INFO, (host_info_t)&vmStat, &vmCount) != KERN_SUCCESS)
    {
        throw Errors::IOException("call to host_processor_info failed!");
    }
    
    // TODO: update Memory info (page in, page out, etc)
    //_memory->_free = vmStat.free_count * vm_page_size;
    _memory->_used = (vmStat.active_count * vm_page_size) + (vmStat.wire_count * vm_page_size);
    _memory->_free = _memory->_total - _memory->_used;
    
    // we will update the swap memory totals here too
    struct xsw_usage swapUsage;
    ::memset(&swapUsage, 0, sizeof(swapUsage));
    int mib[2];
    mib[0] = CTL_VM;
    mib[1] = VM_SWAPUSAGE;
    size_t len = sizeof(swapUsage);
    if (::sysctl(mib, 2, &swapUsage, &len, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for VM_SWAPUSAGE!");   
    _swap->_used = swapUsage.xsu_used;
    _swap->_free = _swap->_total - _swap->_used;
    
    // this is an ok place to update the uptime
    struct timeval result;
    mib[0] = CTL_KERN;
    mib[1] = KERN_BOOTTIME;
    len = sizeof(result);
    if (::sysctl(mib, 2, &result, &len, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for KERN_BOOTTIME!");
    TimeValue current_time;
    System::GetCurrentTime(current_time);
    _uptime = current_time.tv_sec - result.tv_sec;
    _str_uptime.clear();
}


#endif

