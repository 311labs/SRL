/*********************************************************************************
* Copyright (C) 1999,2004 by SRL, Inc.   All Rights Reserved.        *
*********************************************************************************
* Authors: Ian C. Starnes, Dr. J. Shane Frasier, Shaun Grant
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*******************************************************************************                                                     
* Last $Author: ians $     $Revision: 283 $                                
* Last Modified $Date: 2006-09-22 17:04:28 -0400 (Fri, 22 Sep 2006) $                                  
******************************************************************************/
#include <iostream>
#include "srl/sys/ProcessList.h"
#include "srl/sys/SystemInfo.h"
#include "srl/sys/CpuInfo.h"

#include <vector>
#include <algorithm>

#include "srl/Console.h"
#include "srl/Timer.h"

using namespace SRL;
using namespace SRL::System;

#define MEGABYTE 1048576.0f

enum PROC_ACTION
{
	PROC_UNKNOWN,
	PROC_LIST,
	PROC_WATCH,
	PROC_MONITOR,
	PROC_TOP,
	PROC_SYSINFO,
	PROC_CPUINFO,
	PROC_KILL
};

PROC_ACTION proc_action = PROC_UNKNOWN;

bool do_repeat = false;
bool do_top = false;
bool by_pid = true;
bool by_name = true;
bool by_key = false;
uint64 my_pid = System::GetPid();

std::vector<String> proc_names;
std::vector<int> proc_ids;

SRL::String PROG_NAME;

void usage()
{
	int major, minor, revision;
	SRL::GetVersion(major, minor, revision);
	Console::formatLine("SRL Library Version %d.%d.%d\n", major, minor, revision);
	Console::write(PROG_NAME);
	Console::writeLine(" [options] [pids] [names] [keywords]");
	Console::writeLine("\t-l --list\tlist the current running processes");
	Console::writeLine("\t-t --top\tlist the current active processes");
	Console::writeLine("\t-s --system\tlist current system information");
	Console::writeLine("\t-w --watch\twatch process/s or pid/s");
	Console::writeLine("\t-m --monitor\tmonitor process/s or pid/s");
	Console::writeLine("\t-r       \tcontinuously update the screen");
	Console::writeLine("\t-c --cpuid\tshow cpuid information");
	Console::writeLine("\t-k --kill\tkill a process by pid or name");
	Console::writeLine("\t--pid    \tretrieve process by pid/s");
	Console::writeLine("\t--name\tretrieve process by name");
	Console::writeLine("\t--key   \tretrieve process by keyword");
	Console::writeLine("-------------Examples---------------------");
	Console::writeLine("list all process named java:\n\tproclist -l --name java");
	Console::writeLine("list all process named java and keep updating the display:\n\tproclist -lr --name java");
	Console::writeLine("watch process/s with these pids: proclist -w --pid 1021 1018 432");
	Console::writeLine("kill a java process running this class(no worky on windows!): proclist -k --key jsrl.CpuTest");
	exit(1);
}

void parseFullOption(const char* coption)
{
	String option = coption;

	if (option == "--list")
	{
		proc_action = PROC_LIST;
	}
	else if (option == "--watch")
	{
		proc_action = PROC_WATCH;
	}
	else if (option == "--monitor")
	{
		proc_action = PROC_WATCH;
	}	
	else if (option == "--system")
	{
		proc_action = PROC_SYSINFO;
	}
	else if (option == "--top")
	{
		do_top = true;
		do_repeat = true;
		proc_action = PROC_LIST;
	}
	else if (option == "--kill")
	{
		proc_action = PROC_KILL;
	}
	else if (option == "--pid")
	{
		by_pid = true;
		by_name = false;
		by_key = false;
	}
	else if (option == "--name")
	{
		by_name = true;
		by_key = false;
	}
	else if (option == "--key")
	{
		by_name = false;
		by_key = true;
	}
	else if (option == "--cpuid")
	{
		proc_action = PROC_CPUINFO;
	}
	else
	{
		Console::writeLine("unknown option: " + option);
	}
}

void parseOption(const char* opt)
{
	int len = strlen(opt);
	for (int x=1; x<len;++x)
	{
		switch (opt[x])
		{
			case ('t'):
				do_top = true;
				do_repeat = true;
			case ('l'):
				proc_action = PROC_LIST;
				break;
			case ('w'):
				proc_action = PROC_WATCH;
				do_repeat = true;
				break;
			case ('m'):
				proc_action = PROC_MONITOR;
				do_repeat = true;
				break;
			case ('s'):
				proc_action = PROC_SYSINFO;
				break;
			case ('k'):
				proc_action = PROC_KILL;
				break;
			case ('r'):
				do_repeat = true;
				break;
			case ('c'):
				proc_action = PROC_CPUINFO;
				break;
			case ('h'):
				break;
			default:
				Console::formatLine("unknown option: %c", opt[x]);
				break;
		}
	}
}

void parseArgs(int argc, char** argv)
{
	for (int i=1; i<argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == '-')
			{
				parseFullOption(argv[i]);
			}
			else
			{
				parseOption(argv[i]);
			}
		}
		else
		{
			String proc = argv[i];
			if (proc.isDigits())
				proc_ids.push_back(String::Int(proc));
			else
				proc_names.push_back(proc);
		}
	}
}

void sysHeader()
{
	static bool firstTime = true;
	System::SystemInfo *sysinfo;
	System::ProcessList *proc_list;
	try
	{
    	sysinfo = System::SystemInfo::GetInstance();
	    proc_list = System::ProcessList ::GetInstance();
    	if (firstTime)
    	{
    		System::Sleep(1000);
    		firstTime = false;
    	}
    	sysinfo->refresh();
    }
    catch(SRL::Errors::Exception &e)
    {
        Console::writeLine(e.message().text());
        return;
    }

	//Console::clear();
	Console::formatLine("login: %s  hostname: %s  domain: %s", System::GetUserName().text(),
	    sysinfo->name().text(), sysinfo->domain().text());
	Console::formatLine("OS\n\t%s  kernel: %s  version: %s",
					sysinfo->os().name().text(),
					sysinfo->os().kernel().text(),
					sysinfo->os().version().text());
	Console::writeLine("CPU");
	Console::formatLine("\tInfo: %d x %s (%0.4f GHz)", sysinfo->cpuCount(),
					sysinfo->cpu().modelName().text(),
					sysinfo->cpu().speed());
	Console::formatLine("\tPhysical: %d\tLogical: %d\tCores: %d",
				sysinfo->numberOfPhysicalCPUs(),
				sysinfo->numberOfLogicalCPUs(),
				sysinfo->numberOfCoresPerCPU());

	if (sysinfo->isHyperThreadSupported())
	{
		Console::write("\tHyperThreading Supported: True");
		if (sysinfo->isHyperThreadEnabled())
			Console::writeLine("\tEnabled: True");
		else
			Console::writeLine("\tEnabled: False");
	}

	Console::formatLine("\tAverage-> Total: %0.2f%%\tUser: %0.2f%%\tSystem: %0.2f%%",
					sysinfo->cpu().load(), sysinfo->cpu().userLoad(),
					sysinfo->cpu().kernelLoad());
	if (sysinfo->cpuCount() > 1)
	{
	    for (int i=0; i<sysinfo->cpuCount();++i)
	    {
        	Console::formatLine("\t  cpu %d-> Total: %0.2f%%\tUser: %0.2f%%\tSystem: %0.2f%%", i+1,
        					sysinfo->getCPU(i).load(), sysinfo->getCPU(i).userLoad(),
        					sysinfo->getCPU(i).kernelLoad());	        
	    }
	}
	Console::formatLine("Memory\n\tPhysical Total: %02.f MB\t used: %02.f MB\t free: %02.f MB",
					sysinfo->memory().total() / MEGABYTE, sysinfo->memory().used() / MEGABYTE,
					sysinfo->memory().free() / MEGABYTE);
	Console::formatLine("\tSwap Total: %02.f MB\t used: %02.f MB\t free: %02.f MB",
					sysinfo->swap().total() / MEGABYTE, sysinfo->swap().used() / MEGABYTE,
					sysinfo->swap().free() / MEGABYTE);
					
	Console::formatLine("Processes\n\t%u total", proc_list->size());
					
}

void tableHeader()
{
	// table header
	Console::formatLine("%15s %8s %8s %7s %10s %10s %8s %8s %-20s", "name", "id", "parent",
		"cpu", "rss", "vss", "threads", "handles", "  owner");
	Console::writeLine("-------------------------------------------------------------------------------");
}

void printProc(System::ProcessList::Process *proc, bool showBytes=false)
{
	if (!showBytes)
	{
		Console::formatLine("%15s %8d %8d %6.1f%% %8.2fmb %8.2fmb %8d %8d   %-20s",
			proc->name().text(), proc->id(), proc->parentId(),
			proc->cpu().load(), (float)proc->memory().residentSize()/ MEGABYTE,(float)proc->memory().virtualSize()/ MEGABYTE,
			(int)proc->threads(), (int)proc->handles(),
			proc->owner().text());
	}
	else
	{
		Console::formatLine("%15s %8d %8d %6.1f%% %0.4f kb %0.4f kb %8d %8d   %-20s",
			proc->name().text(), proc->id(), proc->parentId(),
			proc->cpu().load(), (float)proc->memory().residentSize()/1024.0f, (float)proc->memory().virtualSize()/1024.0f,
			(int)proc->threads(), (int)proc->handles(),
			proc->owner().text());
	}
}

void findAllProcesses(const String& value)
{
	System::ProcessList  &proc_list = *(System::ProcessList ::GetInstance());
	System::ProcessList::Iterator iter = proc_list.begin();
	bool add_proc = false;
	while (iter != proc_list.end())
	{
		System::ProcessList::Process *proc = iter.value();
		add_proc = false;
		if (by_name)
		{
			if (proc->name() == value)
			{
				add_proc = true;
			}
		}
		else if (by_key)
		{
			if (proc->command().count(value) > 0)
			{
				// ignore my own process
				if (proc->id() != my_pid)
					add_proc = true;
			}
		}

		if (add_proc)
		{
			std::vector<int>::iterator ipid = std::find(proc_ids.begin(), proc_ids.end(), proc->id());
			if (ipid == proc_ids.end())
				proc_ids.push_back(proc->id());
		}
		++iter;
	}
}

void convertNamesToPids()
{
	std::vector<String>::iterator ivalue = proc_names.begin();
	while (ivalue != proc_names.end())
	{
		findAllProcesses(*ivalue);
		++ivalue;
	}
}

void watchList()
{
	if ((proc_ids.size() == 0) && (proc_names.size() == 0))
	{
		usage();
	}

	System::ProcessList  &proc_list = *(System::ProcessList::GetInstance());
	proc_list.refresh();

	convertNamesToPids();

	std::vector<int>::iterator ipid = proc_ids.begin();
	while (ipid != proc_ids.end())
	{
		System::ProcessList::Process *proc = proc_list.getProcess(*ipid);
		if (proc != NULL)
		{
			printProc(proc, true);
			++ipid;
		}
		else
		{
			Console::formatLine("%d process exited!", *ipid);
			ipid = proc_ids.erase(ipid);
		}
	}
}


void procList()
{
	if ((proc_ids.size() > 0) || (proc_names.size() > 0) || (proc_action == PROC_WATCH))
	{
		watchList();
		return;
	}

	System::ProcessList  &proc_list = *(System::ProcessList::GetInstance());
	proc_list.refresh();
	
	System::ProcessList::Iterator iter = proc_list.begin();
    
	while (iter != proc_list.end())
	{
		System::ProcessList::Process *proc = iter.value();
		++iter;

		if (do_top)
			if ((proc->cpu().load() == 0.0) || (proc->id() == 0))
				continue;
		printProc(proc);
	}
}

void sysInfo()
{
    System::SystemInfo *sysinfo;
    try
    {
	    sysinfo = System::SystemInfo::GetInstance();
	    sysinfo->refresh();
    }
    catch(SRL::Errors::Exception &e)
    {
        Console::writeLine(e.message().text());
        return;
    }
	Console::writeLine("--------------- System Info ----------------");
	Console::formatLine("HostName: %s", sysinfo->name().text());
	Console::formatLine("Domain: %s", sysinfo->domain().text());

	Console::writeLine("--------------- OS Info ----------------------");
	Console::formatLine("OS: %s", sysinfo->os().name().text());
	Console::formatLine("Version: %s", sysinfo->os().version().text());
	Console::formatLine("Kernel: %s", sysinfo->os().kernel().text());

	Console::writeLine("--------------- CPU Info ---------------------");
	Console::formatLine("CPU Physical: %d  Logical: %d", sysinfo->numberOfPhysicalCPUs(), sysinfo->numberOfLogicalCPUs());
	Console::formatLine("CPU Model: %s", sysinfo->cpu().modelName().text());
	Console::formatLine("CPU Vendor: %s", sysinfo->cpu().vendor().text());
	Console::formatLine("CPU Speed: %0.4f Ghz", sysinfo->cpu().speed());
	Console::formatLine("CPU Cores: %d" , sysinfo->numberOfCoresPerCPU());
	if (sysinfo->isHyperThreadSupported())
	{
		Console::writeLine("CPU HyperThreading Supported: True");
		if (sysinfo->isHyperThreadEnabled())
			Console::writeLine("CPU HyperThreading Enabled: True");
		else
			Console::writeLine("CPU HyperThreading Enabled: False");
	}
	else
	{
		Console::writeLine("CPU HyperThreading Supported: False");
	}
	Console::writeLine("--------------- CPU Load --------------------");
	Console::formatLine("CPU Load: %0.1f", sysinfo->cpu().load());
	Console::formatLine("CPU User: %0.1f", sysinfo->cpu().userLoad());
	Console::formatLine("CPU Kernel: %0.1f", sysinfo->cpu().kernelLoad());
	if (sysinfo->cpuCount() > 1)
	{
		for (int i=0; i < sysinfo->cpuCount(); ++i)
		{
			const System::SystemInfo::CPU &cpu = sysinfo->getCPU(i);
			Console::formatLine("--------------- CPU %d --------------------", i);
			Console::formatLine("CPU Load: %0.1f", cpu.load());
			Console::formatLine("CPU User: %0.1f", cpu.userLoad());
			Console::formatLine("CPU Kernel: %0.1f", cpu.kernelLoad());
		}
	}

	Console::writeLine("--------------- Memory (Physical) ---------");
	Console::formatLine("Memory Total: %u", sysinfo->memory().total() / MEGABYTE);
	Console::formatLine("Memory Used: %u", sysinfo->memory().used() / MEGABYTE);
	Console::formatLine("Memory Free: %u", sysinfo->memory().free() / MEGABYTE);

	Console::writeLine("--------------- Memory (Virtual) -----------");
	Console::formatLine("Swap Total: %u", sysinfo->swap().total() / MEGABYTE);
	Console::formatLine("Swap Used: %u", sysinfo->swap().used() / MEGABYTE);
	Console::formatLine("Swap Free: %u", sysinfo->swap().free() / MEGABYTE);
}

void actionRepeat()
{
    try
    {
        Console::writeLine("start loading stats..");
	    System::SystemInfo::GetInstance();
	    System::ProcessList::GetInstance();
	    Console::writeLine("done");
	}
    catch(SRL::Errors::Exception &e)
    {
        Console::writeLine("e.message().text()");
        return;
    }
    
	System::Sleep(1000);
	do
	{
		if (do_repeat)
		{
			//Console::clear();
			sysHeader();
		}
	
		switch (proc_action)
		{
			case (PROC_WATCH):
			case (PROC_LIST):
				tableHeader();
				procList();
				break;
			case (PROC_SYSINFO):
				if (!do_repeat)
					sysHeader();
				//sysInfo();
				break;
		}
		
		if (do_repeat)
		{
			System::Sleep(1000);
		}
	}
	while (do_repeat);
}


bool kill(System::ProcessList::Process *proc)
{
	System::ProcessList  &proc_list = *(System::ProcessList::GetInstance());

	Console::format("killing: %s [%d]", proc->name().text(), proc->id());

	int id = proc->id();
	proc->terminate(true);
	
	for (int w=0; w<20; ++w)
	{
		Console::write(".");
		System::Sleep(200);
		
		proc_list.refresh();
		if (proc_list.getProcess(id) == NULL)
		{
			id = -1;
			Console::writeLine("done");
			break;
		}
	}
	if (id != -1)
	{
		Console::writeLine("failed");
	}
	return true;
}

void killProcs()
{
	convertNamesToPids();
	System::ProcessList  &proc_list = *(System::ProcessList::GetInstance());
	std::vector<int>::iterator ipid = proc_ids.begin();
	while (ipid != proc_ids.end())
	{
		System::ProcessList::Process *proc = proc_list.getProcess(*ipid);
		kill(proc);
		++ipid;
	}
}

void test()
{
	while(true)
	{
		System::SystemInfo *sysinfo = System::SystemInfo::GetInstance();
		sysinfo->refresh();

		Console::writeLine("--------------- System Info ----------------");
		Console::writeLine("--------------- CPU Load --------------------");
		Console::formatLine("CPU Load: %0.1f", sysinfo->cpu().load());
		Console::formatLine("CPU User: %0.1f", sysinfo->cpu().userLoad());
		Console::formatLine("CPU Kernel: %0.1f", sysinfo->cpu().kernelLoad());
		if (sysinfo->cpu().load() > 50.0)
		{
			//Console::read();
		}

		if (sysinfo->cpuCount() > 1)
		{
			for (int i=0; i < sysinfo->cpuCount(); ++i)
			{
				const System::SystemInfo::CPU &cpu = sysinfo->getCPU(i);
				if (cpu.load() > 50.0)
				{
					//Console::read();
				}
				Console::formatLine("--------------- CPU %d --------------------", i);
				Console::formatLine("CPU Load: %0.1f", cpu.load());
				Console::formatLine("CPU User: %0.1f", cpu.userLoad());
				Console::formatLine("CPU Kernel: %0.1f", cpu.kernelLoad());
			}
		}
		System::Sleep(900);
	}
}


void showCpuInfo()
{
    uint32 maxi, eax, ebx, edx, unused;
	uint32 logical_cpus = System::GetCpuCount();
	

    CpuSet orig_affinity;
    GetProcessAffinity(orig_affinity);
    uint32 counter = 0;
    SRL::byte package_ids[64];
    for( uint32 i = 0; i < logical_cpus; i += 1 )
    {
        CpuSet cpu_set;
        cpu_set.add(i);
        SetProcessAffinity(cpu_set);
        Console::formatLine("\n=== logical cpu index: %d ===", i);
		// get the max function value  (value we set eax when calling cpuid)
		System::cpuid(0,&maxi,&unused,&unused,&unused);
	    
		SRL::byte apic_id = CpuInfo::GetActiveApicId();
		uint32 logical_per_package = CpuInfo::LogicalProcessorsPerPackage();
		uint32 cores_per_package = CpuInfo::CoresPerPackage();
		uint32 max_logical_per_core = logical_per_package / cores_per_package;    
	    
		Console::formatLine("vendor id: %s", CpuInfo::GetVendorId().text());
		Console::formatLine("cpuid level: %d", maxi);
		Console::format("processor: %x", apic_id);
		Console::format("   package id: %x", CpuInfo::GetActivePackageId(apic_id, logical_per_package));
		Console::format("   core id: %x", CpuInfo::GetActiveCoreId(apic_id, logical_per_package, max_logical_per_core));
		Console::formatLine("   smt id: %x", CpuInfo::GetActiveSmtId(apic_id, max_logical_per_core));

		cpuid(1, &eax, &unused, &unused, &edx);
		uint32 family, model, stepping, type;
		stepping = eax & 0xf;
		model = (eax >> 4) & 0xf;
		family = (eax >> 8) & 0xf;
		type = (eax >> 12) & 0x3;
	    
	    
		Console::format("family: %d", family);
		Console::format("    model: %d", model);
		Console::formatLine("    type: %d", type);
		Console::formatLine("model name: %s", CpuInfo::GetModelName().text());
		Console::formatLine("signature: %s", CpuInfo::GetSignature().text());
		Console::formatLine("stepping: %d", stepping);
		if (CpuInfo::SupportsMultiThreading())
			Console::writeLine("multi-thread support: true");
		else
			Console::writeLine("multi-thread support: false");
	    
		Console::formatLine("physical cpus: %d", CpuInfo::NumberOfPhysicalProcessors());
		Console::formatLine("available logical cpus: %d", logical_cpus);
		Console::formatLine("logical cpus per package: %d", logical_per_package);
		Console::formatLine("cores per package: %d", cores_per_package);
	}
	SetProcessAffinity(orig_affinity);
    // if (CpuInfo::IsIntel())
    //     Console::writeLine("intel: true");
    // else
    //     Console::writeLine("intel: false");
    

}

void monitorProc()
{
    System::ProcessList  &proc_list = *(System::ProcessList::GetInstance());
    
    // repeat until cancled
    System::ProcessList::Process *process = NULL;
    if (proc_ids.size() > 0)
    {
        process = proc_list.getProcess(proc_ids[0]);
    }
    else if (proc_names.size() > 0)
    {
        process = proc_list.getProcess(proc_names[0], true);        
    }

    if (process == NULL)
    {
        Console::writeLine("could not find process");
        return;
    }
    uint32 pid = process->id();
    uint32 rss_low = process->memory().residentSize();
    uint32 rss_high = process->memory().residentSize();
    uint32 rss_last = process->memory().residentSize();
    int64 rss_delta = 0;
    while (process != NULL)
    {
        Console::clear();
        Console::formatLine("Process: %s\tPid: %u", process->name().text(), process->id());
        Console::formatLine("\tcpu: %0.2f\tthreads: %u\thandles: %u", process->cpu().load(), 
            process->threads(), process->handles());
        uint32 mem_bytes = process->memory().residentSize();
        Console::formatLine("   Memory:\n\tresident: %u bytes | %0.4f kilobytes | %0.6f megabytes", 
            mem_bytes, mem_bytes/1204.0f, mem_bytes/MEGABYTE);
        
        if (mem_bytes < rss_low)
            rss_low = mem_bytes;
        if (mem_bytes > rss_high)
            rss_high = mem_bytes;
        if (mem_bytes != rss_last)
        {
            rss_delta = static_cast<int64>(mem_bytes) - static_cast<int64>(rss_last);
            rss_last = mem_bytes;
        }   
        
        Console::formatLine("\t\tlow: %u bytes\thigh: %u\tdelta: %d", rss_low, rss_high, rss_delta);
        
        mem_bytes = process->memory().virtualSize();
        Console::formatLine("\tvirtual: %u bytes | %0.4f kilobytes | %0.6f megabytes", 
            mem_bytes, mem_bytes/1204.0f, mem_bytes/MEGABYTE);
        Console::formatLine("\tfaults: %u\tpageins: %u", process->memory().faults(), process->memory().pageins());
        System::Sleep(1000);
        proc_list.refresh();
        process = proc_list.getProcess(pid);
    }
    
    
}

int main(int argc, char** argv)
{
	PROG_NAME = argv[0];
	if ( PROG_NAME.count('\\') > 0 )
	{
		PROG_NAME = PROG_NAME.substr(PROG_NAME.rfind('\\')+1);
	}
	else if ( PROG_NAME.count('/') > 0 )
	{
		PROG_NAME = PROG_NAME.substr(PROG_NAME.rfind('/')+1);
	}

	if (argc <= 1)
		usage();

	parseArgs(argc, argv);

	if (proc_action == PROC_UNKNOWN)
		usage();

    try
    {
    	if (proc_action == PROC_CPUINFO)
    		showCpuInfo();
    	else if (proc_action == PROC_KILL)
    		killProcs();
    	else if (proc_action == PROC_MONITOR)
            monitorProc();
    	else
    		actionRepeat();
    }
    catch(...)
    {
        Console::err::dumpStack();
    }

}

