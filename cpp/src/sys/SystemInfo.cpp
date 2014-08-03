
#include "srl/sys/SystemInfo.h"
#include "srl/sys/CpuInfo.h"

using namespace SRL;
using namespace SRL::System;

SystemInfo::CPU::CPU(uint16 number, bool is_avg) :
_mutex(), _number(number), _freq(0.0), _vendor_id(),
_model_name(), _cache(0), _user(0), _kernel(0), _nice(0), _idle(0),
_load(0.0f), _user_load(0.0f), _kernel_load(0.0f),
_last_update(0.0), _last_clocks(0), _is_avg(is_avg),
_apic_id(0), _smt_id(0), _core_id(0), _package_id(0),
_model(0), _family(0), _type(0)
{

}

void SystemInfo::CPU::_refreshSpeed(const float64 &elapsed_time)
{
    uint64 new_clocks = CurrentCpuClocks();
    float64 clocks = (new_clocks - _last_clocks)/elapsed_time;
    _freq = clocks/1000000000.0;
    _last_clocks = new_clocks;
}

void SystemInfo::CPU::refresh()
{
    // only allow updates every 1 second at most
    float64 cur_time = GetSystemTimer();
    float64 delta_time = (cur_time - _last_update);
    _last_update = cur_time;
    ScopedLock scoped(_mutex);

    if ((_number == 0) || (_number == 1))
        _refreshSpeed(delta_time);

    this->update_times();
}


// SytemInfo Logic
SystemInfo::SystemInfo() : 
_cpu_avg(NULL),
_cpus(),
_memory(NULL),
_swap(NULL),
_os(),
_host_name(),
_domain(),
_num_logical_cpus(0),
_num_physical_cpus(0),
_cores_per_cpu(0),
_ht_support(false),
_ht_enabled(false),
_uptime(0), _str_uptime(56), _last_update(0.0)
{
    _memory = new Memory();
    _swap = new Memory();
    _update_os();
    _update_cpus();
    _update_memory();
    //SRL::Sys::Sleep(1000);
    this->refresh();
}

SystemInfo::~SystemInfo()
{
    if (_cpus.size() > 1)
        delete _cpu_avg;
        
    _cpus.clear(true);
    delete _memory;
    delete _swap;
}

void SystemInfo::refresh()
{
    // only allow updates every 1 second at most
    float64 cur_time = GetSystemTimer();
    float64 delta_time = (cur_time - _last_update);
    
    if (delta_time < 1.0f)
        return;
        
    _last_update = cur_time;        
    for (uint32 i=0; i < _cpus.size(); ++i)
        _cpus[i]->refresh();
    
    if (_cpus.size() > 1)
        _cpu_avg->refresh();
        
    _update_memory();
}

void SystemInfo::_update_cpu_info()
{
    // _num_physical_cpus, _num_logical_cpus, _cores_per_cpu, _ht_support, _ht_enabled
    SRL::byte apic_id = CpuInfo::GetActiveApicId();
    _num_logical_cpus = _cpus.size();
	uint32 logical_per_package = CpuInfo::LogicalProcessorsPerPackage();
	_cores_per_cpu = CpuInfo::CoresPerPackage();
	uint32 max_logical_per_core = logical_per_package / _cores_per_cpu;
    CpuSet orig_affinity;
    GetProcessAffinity(orig_affinity);
    // first calculate the number of physical packages
    // we do this by comparing the package-ids, so we need the ability to run on each processor
    for (uint32 i=0; i < _num_logical_cpus; ++i)
    {
        // first thing we want to do is set this process to run on the 
        // logical processor
        CpuSet cpu_set;
        cpu_set.add(i);
        SetProcessAffinity(cpu_set);
        
        _cpus[i]->_smt_id = CpuInfo::GetActiveSmtId(apic_id, max_logical_per_core);
        _cpus[i]->_core_id = CpuInfo::GetActiveCoreId(apic_id, logical_per_package, max_logical_per_core);
        _cpus[i]->_package_id = CpuInfo::GetActivePackageId(apic_id, logical_per_package);
        bool have_it = false;
        for( uint32 j = 0; j < i; ++j)
        {
            if (_cpus[i]->_package_id == _cpus[j]->_package_id)
            {
                have_it = true;
                break;
            }
        }
        
        if (!have_it)
            _num_physical_cpus += 1;
    }
    // set the process affinity back to the old affinity mask
    SetProcessAffinity(orig_affinity);
    
    // calculate the number of available cores on the system
    // even on a single core system this should atleast be the same as the physical count
    uint32 total_cores = 0;
    for (uint32 i=0; i < _num_logical_cpus; ++i)
    {        
        bool have_it = false;
        for( uint32 j = 0; j < i; ++j)
        {
            if (_cpus[i]->_core_id == _cpus[j]->_core_id)
            {
                have_it = true;
                break;
            }
        }
        
        if (!have_it)
            total_cores += 1;
    }
    
	// GenuineIntel if there are more cores then physical cpus then we know the system is multicore
	if (_cpus[0]->_vendor_id == "GenuineIntel")
	{
		if (total_cores > _num_physical_cpus)
		{
			// multi-core system
			if (max_logical_per_core == 1)
			{
				_ht_support = false;
				_ht_enabled = false;
			}
			else
			{
				_ht_support = true;
				if ((_num_logical_cpus - total_cores) > _num_physical_cpus)
					_ht_enabled = true;
				else
					_ht_enabled = false;
			}
		}
		else
		{
			// single-core system
			// multi-core system
			if (max_logical_per_core == 1)
			{
				_ht_support = false;
				_ht_enabled = false;
			}
			else
			{
				_ht_support = true;
				if (_num_logical_cpus > _num_physical_cpus)
					_ht_enabled = true;
				else
					_ht_enabled = false;
			}
		}
		
	}
	else if (_cpus[0]->_vendor_id == "AuthenticAMD")
	{
		_ht_support = false;
		_ht_enabled = false;
		if (_num_logical_cpus > _num_physical_cpus)
		{
			_cores_per_cpu = max_logical_per_core;
		}
	}    
    
}


const String& SystemInfo::strUptime()
{
    if (_str_uptime.isEmpty())
    {    
        uint32 secs = _uptime % 60;
        uint32 mins = _uptime / 60 % 60;
        uint32 hours = _uptime / 60 / 60 % 24;
        uint32 days = _uptime / 60 / 60 / 24;
        
        if (days > 0)
        {
            if (days == 1)
                _str_uptime +="1 day, ";
            else
                _str_uptime += String::Format("%d days, ", days);
        }

        _str_uptime += String::Format("%02d:%02d:%02d", hours, mins, secs);
    }
    
    return _str_uptime;
}
