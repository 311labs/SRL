#include "srl/sys/SystemInfo.h"
#include "srl/sys/CpuInfo.h"
#include "srl/sys/SystemQuery.h"

#ifdef WIN32
#include <tchar.h>

using namespace SRL;
using namespace SRL::System;

// ------------ Class SystemInfo ----------------
SYSTEM_LOAD_INFORMATION* update_cpu_avg(int cpu_count, SYSTEM_LOAD_INFORMATION* cpu, SYSTEM_LOAD_INFORMATION* cpu_info)
{

    for (int i=0;i<cpu_count;++i)
    {
        cpu->kernel_time += cpu_info[i].kernel_time;
        cpu->user_time += cpu_info[i].user_time;
        cpu->idle_time += cpu_info[i].idle_time;
    }
    return cpu;
}



// ------------ Class System CPU ----------------
void SystemInfo::CPU::update_times()
{
    SYSTEM_LOAD_INFORMATION cpu;
    cpu.idle_time = 0;
    cpu.kernel_time = 0;
    cpu.user_time = 0;
    SystemQuery query(QUERY_CPU_LOAD);
    SYSTEM_LOAD_INFORMATION* cpu_info =
            (SYSTEM_LOAD_INFORMATION*)query.buffer;

    if (_is_avg)
    {
        int cpu_count = _number;
        if (cpu_count == 0)
            ++cpu_count;
    
        for (int i=0;i< cpu_count;++i)
        {
            cpu_info[i].kernel_time = cpu_info[i].kernel_time / 100000;
            cpu_info[i].user_time = cpu_info[i].user_time / 100000;
            cpu_info[i].idle_time = cpu_info[i].idle_time / 100000;
            cpu_info[i].kernel_time = cpu_info[i].kernel_time - cpu_info[i].idle_time;
        }
        cpu_info = update_cpu_avg(cpu_count, &cpu, cpu_info);
    }
    else
    {
        cpu_info = &cpu_info[_number];
        cpu_info->kernel_time = cpu_info->kernel_time / 100000;
        cpu_info->user_time = cpu_info->user_time / 100000;
        cpu_info->idle_time = cpu_info->idle_time / 100000;
        cpu_info->kernel_time = cpu_info->kernel_time - cpu_info->idle_time;
    }

    // TODO: BUG: Ians: This is a hack to fix a strange bug with the kernel time shrinking instead of growing
    if (cpu_info->kernel_time < _kernel)
        cpu_info->kernel_time = _kernel;

    if (_last_update != 0.0f)
    {
        uint32 delta_user = static_cast<uint32>(cpu_info->user_time) - _user;
        uint32 delta_kernel = static_cast<uint32>(cpu_info->kernel_time) - _kernel;
        uint32 delta_idle = static_cast<uint32>(cpu_info->idle_time) - _idle;

        if (_last_update != 0.0f)
        {   
            uint32 load_total = delta_user + delta_kernel;
            uint32 cpu_total  = load_total + delta_idle;
            _load = 100.0f * (float32)load_total/(float32)cpu_total;
            _user_load = 100.0f * ((float32)delta_user)/(float32)cpu_total;
            _kernel_load = 100.0f * (float32)delta_kernel/(float32)cpu_total;
        }
        else
        {
            _load = 0.0f;
            _user_load = 0.0f;
            _kernel_load = 0.0f;
        }
    }

    _user = static_cast<uint32>(cpu_info->user_time);
    _kernel = static_cast<uint32>(cpu_info->kernel_time);
    _idle = static_cast<uint32>(cpu_info->idle_time);
}

void SystemInfo::_update_os()
{
    char buffer[256] = "";
    DWORD buffer_size = 256;
    ::GetComputerNameEx((COMPUTER_NAME_FORMAT)1, buffer, &buffer_size);
    _host_name = buffer;
    ::GetComputerNameEx((COMPUTER_NAME_FORMAT)2, buffer, &buffer_size);
    _domain = buffer;
    
    // get OS Info
    OSVERSIONINFOEX os_info;
    os_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    bool got_info = true;
    if (!::GetVersionEx((OSVERSIONINFO*)&os_info))
    {
        os_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        if (!::GetVersionEx((OSVERSIONINFO*)&os_info))
            got_info = false;
    }
    
    _os._name = "Windows";
    if (got_info)
    {
        switch (os_info.dwPlatformId)
        {
        case (VER_PLATFORM_WIN32_NT):
            if (os_info.dwMajorVersion == 6)
                _os._version = "Vista";
            else if ((os_info.dwMajorVersion == 5) && (os_info.dwMinorVersion == 2))
                _os._version = "Server 2003";
            else if ((os_info.dwMajorVersion == 5) && (os_info.dwMinorVersion == 1))
                _os._version = "XP";
            else if ((os_info.dwMajorVersion == 5) && (os_info.dwMinorVersion == 0))
                _os._version = "2000";
            else if (os_info.dwMajorVersion <= 4)
                _os._version = "NT";
            else
                _os._version = "UNKNOWN";
            
            // test for product
            if (os_info.wProductType == VER_NT_WORKSTATION)
            {
                if (os_info.dwMajorVersion == 4)
                    _os._kernel = " Workstation 4.0";
                else if (os_info.wSuiteMask & VER_SUITE_PERSONAL)
                    _os._kernel = " Home Edition";
                else
                    _os._kernel = " Professional";
            }
            else if (os_info.wProductType == VER_NT_SERVER ||
                     os_info.wProductType == VER_NT_DOMAIN_CONTROLLER)
            {
                if(os_info.dwMajorVersion==5 && os_info.dwMinorVersion==2)
                {
                    if( os_info.wSuiteMask & VER_SUITE_DATACENTER )
                        _os._kernel = " Datacenter Edition";
                    else if( os_info.wSuiteMask & VER_SUITE_ENTERPRISE )
                        _os._kernel = " Enterprise Edition";
                    else if ( os_info.wSuiteMask == VER_SUITE_BLADE )
                        _os._kernel = " Web Edition";
                    else
                        _os._kernel = " Standard Edition";
                }
                else if(os_info.dwMajorVersion==5 && os_info.dwMinorVersion==0)
                {
                    if( os_info.wSuiteMask & VER_SUITE_DATACENTER )
                        _os._kernel = " Datacenter Server";
                    else if( os_info.wSuiteMask & VER_SUITE_ENTERPRISE )
                        _os._kernel = " Advanced Server";
                    else
                        _os._kernel = " Server";
                }
                else  // Windows NT 4.0
                {
                    if( os_info.wSuiteMask & VER_SUITE_ENTERPRISE )
                        _os._kernel = " Server 4.0, Enterprise Edition";
                    else
                        _os._kernel = " Server 4.0";
                        }
            
            }
            
        }
        _os._kernel += String::Format(" [%d.%d.%d]", os_info.dwMajorVersion,
                                os_info.dwMinorVersion, os_info.dwBuildNumber);
        String sp = os_info.szCSDVersion;
        if (sp.length() > 0)
            _os._kernel += " " + sp;
    }
}

void SystemInfo::_update_cpus()
{
    // now get the number of cpus
    SYSTEM_INFO sys_info;
    ::GetSystemInfo(&sys_info);
    uint32 cpu_count = static_cast<uint32>(sys_info.dwNumberOfProcessors);
    
    //char buf[256];

    //HKEY hKey;
    //LONG lRet;
    //DWORD dwBufLen = 256;
    //lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
    //    "Hardware\\Description\\System\\CentralProcessor\\0",
    //    0, KEY_QUERY_VALUE, &hKey );
    //
    //if( lRet == ERROR_SUCCESS )
    //{
    //    lRet = RegQueryValueEx( hKey, "ProcessorNameString",
    //                            NULL, NULL, (LPBYTE)buf, &dwBufLen);
    //}

    String vendorId = CpuInfo::GetVendorId();
    String model = CpuInfo::GetModelName();
    uint32 ifamily, imodel, itype, eax, unused;
    
    cpuid(1,&eax,&unused,&unused,&unused);
    imodel = (eax >> 4) & 0xf;
    ifamily = (eax >> 8) & 0xf;
    itype = (eax >> 12) & 0x3;   

    for (uint32 c=0; c<cpu_count; ++c)
    {
        CPU *cpu = new CPU(c);
        cpu->_vendor_id = vendorId;
        cpu->_model_name = model;
		cpu->_freq = System::CurrentCpuFreq()/1000000000.0;
        cpu->_cache = 0;
		cpu->_type = itype;
		cpu->_family = ifamily;
		cpu->_model = imodel;
		_cpus.add(cpu);
    }

    _update_cpu_info();

	if (cpu_count > 1)
    {
        _cpu_avg= new CPU(cpu_count,true);
		_cpu_avg->_vendor_id = _cpus[1]->_vendor_id;
		_cpu_avg->_model_name = _cpus[1]->_model_name;
		_cpu_avg->_model = _cpus[1]->_model;
		_cpu_avg->_family = _cpus[1]->_family;
		_cpu_avg->_type = _cpus[1]->_type;
		_cpu_avg->_freq = _cpus[1]->_freq;
		_cpu_avg->_cache = _cpus[1]->_cache;
    }
	else
    {
        _cpu_avg = _cpus[0];
    }

}

void SystemInfo::_update_memory()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	::GlobalMemoryStatusEx(&statex);
	
	_memory->_total = static_cast<uint32>(statex.ullTotalPhys);
	_memory->_free = static_cast<uint32>(statex.ullAvailPhys);
	_memory->_used = _memory->_total - _memory->_free;
	// TODO ?? paging file or Virtual memory??
	_swap->_total = static_cast<uint32>(statex.ullTotalVirtual);
	_swap->_free = static_cast<uint32>(statex.ullAvailVirtual);
	_swap->_used = _swap->_total - _swap->_free;
	
    _uptime = ::GetTickCount() / 1000;
	
}


#endif

