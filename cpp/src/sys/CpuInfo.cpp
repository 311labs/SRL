#include "srl/sys/CpuInfo.h"

using namespace SRL;
using namespace SRL::System;


// Checks if CPUID instruction is available
bool CpuInfo::SupportsCpuid()
{
    uint32 result = 0;
#ifndef WIN32
#ifndef OSX
	__asm__ __volatile__ (
		"\tpushfl\n"
		"\tpopl %%eax\n"
		"\tmovl %%eax, %%ecx\n"
		"\tandl $0x200000, %%ecx\n"
		"\txorl $0x200000, %%eax\n"
		"\tpushl %%eax\n"
		"\tpopfl\n"
		"\tpushfl\n"
		"\tandl $0x200000, %%eax\n"
		"\txorl %%ecx, %%eax\n"
		"\tmovl %%eax, %0\n"
		: "=m" (result) : : "eax", "ecx"
	);
#else
return true;

#endif
#elif defined(WIN64)
    return true;
#elif defined(__i386__)
    try
    {
        // call cpuid with eax = 0
        __asm
        {
            mov eax, 0
            cpuid
            mov result, eax
        }
    }
    catch (...)
    {
        return false;                   // cpuid instruction is unavailable
    }    
#endif
	return (result != 0);	
}

// This tells us if hyperthreading or multicores are present
// EDX[28]  Bit 28 is set if HT or multi-core is supported
#define HWD_MT_BIT         0x10000000
bool CpuInfo::SupportsMultiThreading()
{
    uint32 edx, unused;
    cpuid(1, &unused, &unused, &unused, &edx);
    return ((edx & HWD_MT_BIT) != 0);	
}



SRL::String CpuInfo::GetVendorId()
{
    uint32 maxi, ecx, ebx, edx;
    System::cpuid(0,&maxi,&ebx,&ecx,&edx);	
	char tmp[16];
	memcpy(tmp, &ebx, sizeof(int));
	memcpy(&(tmp[4]), &edx, sizeof(int));
	memcpy(&(tmp[8]), &ecx, sizeof(int));
    tmp[12] = '\0';
    return tmp;
}

SRL::String CpuInfo::GetModelName()
{
    SRL::String vendor = "";
    
    uint32 eax, ecx, ebx, edx;
    char tmp[17];
	for(uint32 i=0x80000002;i<=0x80000004;i++)
	{
	    System::cpuid(i,&eax,&ebx,&ecx,&edx);	
        tmp[16] = '\0';
        for(int j=0;j<4;j++)
        {
          tmp[j] = eax >> (8*j);
          tmp[j+4] = ebx >> (8*j);
          tmp[j+8] = ecx >> (8*j);
          tmp[j+12] = edx >> (8*j);
        }
        vendor += tmp;
	}    
    return vendor.remove("  ");
}

SRL::String CpuInfo::GetSignature()
{
    uint32 signature, eax, ecx, ebx, edx;
    System::cpuid(1,&signature,&ebx,&ebx,&ebx);
    System::cpuid(3,&eax,&ebx,&ecx,&edx);
    return String::Format("%04X-%04X-%04X-%04X-%04X-%04X",signature >> 16,
                                    signature & 0xffff, edx >> 16, edx & 0xffff,
                                    ecx >> 16, ecx & 0xffff);
}




// returns the number of logical cpus per physical cpu
// EBX[23:16] Bit 16-23 in ebx contains the number of logical
// processors per physical processor when execute cpuid with 
// eax set to 1
#define NUM_LOGICAL_BITS   0x00FF0000     
uint32 CpuInfo::LogicalProcessorsPerPackage()
{
	// if does not support multicores or hyperthreading then its just 1
    if (!CpuInfo::SupportsMultiThreading()) 
        return 1;
    
    uint32 ebx, unused;
    cpuid(1, &unused, &ebx, &unused, &unused);
    return (uint32) ((ebx & NUM_LOGICAL_BITS) >> 16);	    
}

// returns the number of cores per cpu
// EAX[31:26] Bit 26-31 in eax contains the number of cores minus one
// per physical processor when execute cpuid with 
// eax set to 4.
#define NUM_CORE_BITS      0xFC000000  
uint32 CpuInfo::CoresPerPackage()
{
	// if does not support multicores or hyperthreading then its just 1
    if (!CpuInfo::SupportsMultiThreading()) 
        return 1;
    
    uint32 maxi, eax, unused;
    System::cpuid(0,&maxi,&unused,&unused,&unused);	
    if (maxi > 4)
        cpuid(maxi, &eax, &unused, &unused, &unused);
    else
        return 1;
    return (uint32)((eax & NUM_CORE_BITS) >> 26)+1;	    
}

// EBX[31:24] Bits 24-31 (8 bits) return the 8-bit unique 
// initial (Advanced Programmable Interface Controller)APIC ID for the processor this code is running on.
// eax set to 1
#define INITIAL_APIC_ID_BITS  0xFF000000
SRL::byte CpuInfo::GetActiveApicId()
{
    uint32 ebx, unused;
    cpuid(1, &unused, &ebx, &unused, &unused);
    return (SRL::byte) ((ebx & INITIAL_APIC_ID_BITS) >> 24);
}

//
// Determine the width of the bit field that can represent the value count_item. 
//
uint32 find_maskwidth(uint32 CountItem)
{
    uint32 MaskWidth, count = CountItem;
#ifndef WIN32
	asm
	(
		"movl $0, %%ecx\n\t"
		"decl %%eax\n\t"
		"bsrw %%ax, %%cx\n\t"
		"jz next\n\t"
		"incw %%cx\n"
		"next:\n\t"
		"movl %%ecx, %%eax"
		: "=c" (MaskWidth)
		: "a" (count)		
	);
#else
	__asm
	{
		mov eax, count
		mov ecx, 0
		mov MaskWidth, ecx
		dec eax
		bsr cx, ax
		jz next
		inc cx
		mov MaskWidth, ecx
next:
		
	}
#endif

    return MaskWidth;
}

//
// Extract the subset of bit field from the 8-bit value FullID.  It returns the 8-bit sub ID value
//
unsigned char GetNzbSubID(unsigned char FullID,
                          unsigned char MaxSubIDValue,
                          unsigned char ShiftCount)
{
    uint32 MaskWidth;
    unsigned char MaskBits;

    MaskWidth = find_maskwidth((uint32) MaxSubIDValue);
    MaskBits  = (0xff << ShiftCount)^ 
                ((unsigned char) (0xff << (ShiftCount + MaskWidth)));

    return (FullID & MaskBits);
}


SRL::byte CpuInfo::GetActivePackageId(SRL::byte &apic_id, uint32 &logical_per_package)
{
    // Extract package ID, assume single cluster.
    // Shift value is the mask width for max Logical per package
    return apic_id & (unsigned char) (0xff << find_maskwidth(logical_per_package));
}

SRL::byte CpuInfo::GetActiveCoreId(SRL::byte &apic_id, uint32 &logical_per_package, uint32 &max_logical_per_core)
{
    return GetNzbSubID(apic_id, logical_per_package,
                   (unsigned char) find_maskwidth(max_logical_per_core));
}

SRL::byte CpuInfo::GetActiveSmtId(SRL::byte &apic_id, uint32 &max_logical_per_core)
{
    return GetNzbSubID(apic_id, max_logical_per_core, 0);
}

uint32 CpuInfo::NumberOfLogicalProcessors()
{
    return System::GetCpuCount();
}

uint32 CpuInfo::NumberOfPhysicalProcessors()
{
    SRL::byte apic_id = CpuInfo::GetActiveApicId();
    uint32 logical_cpus = System::GetCpuCount();
    uint32 logical_per_cpu = CpuInfo::LogicalProcessorsPerPackage();
    CpuSet orig_affinity;
    GetProcessAffinity(orig_affinity);
    uint32 counter = 0;
    SRL::byte package_ids[64];
    for( uint32 i = 0; i < logical_cpus; i += 1 )
    {
        CpuSet cpu_set;
        cpu_set.add(i);
        SetProcessAffinity(cpu_set);
        
        package_ids[i] = CpuInfo::GetActivePackageId(apic_id, logical_per_cpu);
        bool have_it = false;
        for( uint32 j = 0; j < i; ++j)
        {
            if (package_ids[i] == package_ids[j])
            {
                have_it = true;
                break;
            }
        }
        
        if (!have_it)
            counter += 1;        
    }
    
    // set the process affinity back to the old affinity mask
    SetProcessAffinity(orig_affinity);
    return counter; 
}


#define INTEL_SIGNATURE 0x756e6547
bool CpuInfo::IsIntel()
{
    uint32 ebx, unused;
    cpuid(0, &unused, &ebx, &unused, &unused);
    return (ebx == INTEL_SIGNATURE);
}

#define AMD_SIGNATURE 0x68747541
bool CpuInfo::IsAmd()
{
    uint32 ebx, unused;
    cpuid(0, &unused, &ebx, &unused, &unused);
    return (ebx == AMD_SIGNATURE);
}

