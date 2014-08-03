#include "srl/sys/SystemInfo.h"

#ifdef linux

#include "srl/fs/Directory.h"
#include "srl/fs/TextFile.h"

#include <sys/utsname.h>

using namespace SRL;
using namespace SRL::System;

void SystemInfo::CPU::update_times()
{
	// update
	String file_data;
	FS::CatTextFile("/proc/stat", &file_data);
	int index = _number+1;	    
	if (_is_avg)
	    index = 0;
	String cpu_data = file_data.section('\n', index).replace("  ", " ");

	uint32 new_user = String::Int(cpu_data.section(' ', 1));
	new_user += String::Int(cpu_data.section(' ', 2));
	//uint32 new_nice = String::Int(cpu_data.section(' ', 2));
	uint32 new_kernel = String::Int(cpu_data.section(' ', 3));
	uint32 new_idle = String::Int(cpu_data.section(' ', 4));
	
	if (_last_update != 0.0f)
	{
		uint32 delta_user = new_user - _user;
		uint32 delta_kernel = new_kernel - _kernel;
		//uint32 delta_nice = new_nice - _nice;
		uint32 delta_idle = new_idle - _idle;
	
		uint32 load_total = delta_user + delta_kernel;
		uint32 cpu_total  = load_total + delta_idle;
		_load = 100.0f * (float32)load_total/(float32)cpu_total;
		_user_load = 100.0f * ((float32)delta_user)/(float32)cpu_total;
		//_user_load = 100.0f * (float32)(delta_user+delta_nice)/(float32)cpu_total;
		_kernel_load = 100.0f * (float32)delta_kernel/(float32)cpu_total;
	}

	_user = new_user;
	_kernel = new_kernel;
	//_nice = new_nice;
	_idle = new_idle;
}

void SystemInfo::_update_os()
{
	struct utsname uname_pointer;
	
	uname(&uname_pointer);
	_os._name = uname_pointer.sysname;
	_os._kernel = uname_pointer.release;
	_os._machine = uname_pointer.machine;
	
	_host_name = uname_pointer.nodename;
	_domain = uname_pointer.domainname;   

	// check for linux distro
	FS::Directory etc_dir("/etc");
	for (int i=0; i<etc_dir.totalChildren();++i)
	{
		FS::FsObject *fsobj = etc_dir.getChild(i);
		if ((!fsobj->isDir()) && (fsobj->name().find("-release") > 0))
		{
			String distro;
			FS::CatTextFile(fsobj->fullName(), &distro);
			distro.resize(distro.length()-1);
			_os._version = distro;
		}
	}	
	 
}

void SystemInfo::_update_cpus()
{
    
	// only update the cpus if we haven't done anything yet
	if (_cpus.size()>0)
	    return;

    uint32 ifamily, imodel, itype, eax, unused;
    
    cpuid(1,&eax,&unused,&unused,&unused);
    imodel = (eax >> 4) & 0xf;
    ifamily = (eax >> 8) & 0xf;
    itype = (eax >> 12) & 0x3;
	
	FS::TextFile* cpu_file = FS::OpenTextFile("/proc/cpuinfo");
	while (!cpu_file->atEnd())
	{
		String line;
		cpu_file->readLine(line, true);
		if (line.length() > 0)
		{
			CPU *cpu = new CPU(_cpus.size());
			_cpus.add(cpu);
			uint32 line_count = 0;
			while (line.length() > 0)
			{
				switch (line_count)
				{
				case (1):
					cpu->_vendor_id = line.section("\t: ", 1);
					break;
				case (4):
					cpu->_model_name = line.section("\t: ", 1);
					break;
				case (6):
					cpu->_freq = ((double)CurrentCpuFreq())/1000000000.0;//String::Float(line.section("\t: ", 1));
					break;
				case (7):
					cpu->_cache = String::Int(line.section("\t: ", 1).remove(" KB"));
					break;
				}
				++line_count;
				cpu_file->readLine(line, true);
			}
			
            cpu->_model =  imodel;
            cpu->_family = ifamily;
            cpu->_type = itype;
			
		}
	}
	cpu_file->close();
	
    _update_cpu_info();
	
	// New if we have more then one cpu we need to create a 
	// cpu averager
	if (_cpus.size() > 1)
	{
		_cpu_avg  = new CPU(0);
		_cpu_avg->_vendor_id = _cpus[1]->_vendor_id;
		_cpu_avg->_model_name = _cpus[1]->_model_name;
		_cpu_avg->_model = _cpus[1]->_model;
		_cpu_avg->_family = _cpus[1]->_family;
		_cpu_avg->_type = _cpus[1]->_type;
		_cpu_avg->_freq = _cpus[1]->_freq;
		_cpu_avg->_cache = _cpus[1]->_cache;
		_cpu_avg->_is_avg = true;
	}
	else
		_cpu_avg = _cpus[0];
}

void SystemInfo::_update_memory()
{
    // update the uptime field
    _str_uptime.clear();
    String tmp;
    FS::CatTextFile("/proc/uptime", &tmp);
    _uptime = String::UInt(tmp.section('.', 0));    
    
	FS::TextFile* mem_file = FS::OpenTextFile("/proc/meminfo");
	String line;
	uint32 line_count =1;
	while ((!mem_file->atEnd()) && (line_count < 14))
	{
		mem_file->readLine(line);
		switch (line_count)
		{
		case (1):
			_memory->_total = String::Int(line.section(':',1).remove(' ').remove("kB")) * 1024;
			break;
		case (2):
			_memory->_free = String::Int(line.section(':',1).remove(' ').remove("kB")) * 1024;
			_memory->_used = _memory->_total - _memory->_free;
			break;
		case (6):
		    //_memory->_active = String::Int(line.section(':',1).remove(' ').remove("kB")) * 1024;
		    break;
		case (7):
		    //_memory->_inactive = String::Int(line.section(':',1).remove(' ').remove("kB")) * 1024;
		    break;
		case (12):
			_swap->_total = String::Int(line.section(':',1).remove(' ').remove("kB")) * 1024;
			break;
		case (13):
			_swap->_free = String::Int(line.section(':',1).remove(' ').remove("kB")) * 1024;
			_swap->_used = _swap->_total - _swap->_free;
			break;
		}
		++line_count;
	}
	mem_file->close();
}


#endif // linux

