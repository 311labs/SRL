#include "srl/sys/System.h"
#include "srl/Exceptions.h"

#include <sys/sysctl.h>

using namespace SRL;
using namespace SRL::System;

uint32 System::GetCpuCount()
{
    int mib[4];  
    // lets get the cpu count
    mib[0] = CTL_HW;
    mib[1] = HW_AVAILCPU;
    int cpu_count = 0;
    size_t len = sizeof(cpu_count);
    if (::sysctl(mib, 2, &cpu_count, &len, NULL, 0) < 0)
        throw Errors::IOException("call to sysctl failed for HW_NCPU!");
    return cpu_count;
}

// at this time there is no support for Process Affinity in OS X
System::CpuSet::CpuSet()
{

}

void System::CpuSet::add(const uint32& cpu_index)
{
    
}

void System::CpuSet::remove(const uint32& cpu_index)
{

}

bool System::CpuSet::has(const uint32& cpu_index)
{
    return false;
}

uint32 System::CpuSet::maxSize()
{
    return 0;
}

void System::GetProcessAffinity(CpuSet &affinity_mask, uint64 pid)
{

}

void System::SetProcessAffinity(CpuSet &affinity_mask, uint64 pid)
{
    SRL::System::Sleep(0);
}
