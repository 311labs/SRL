#include "srl/app/ArgumentParser.h"
#include "srl/Console.h"
#include "srl/sys/SystemInfo.h"
#include "srl/sys/CpuInfo.h"

using namespace SRL;
using namespace SRL::System;

void basic_info()
{
    uint32 maxi, eax, ebx, edx, unused;    
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
    Console::formatLine("available logical cpus: %d", System::GetCpuCount());
    Console::formatLine("logical cpus per package: %d", logical_per_package);
    Console::formatLine("cores per package: %d", cores_per_package);
    
    // if (CpuInfo::IsIntel())
    //     Console::writeLine("intel: true");
    // else
    //     Console::writeLine("intel: false");
    

}


void extra_info()
{
        
    uint32 maxi, eax, ecx, ebx, edx, unused;
    // get the max function value  (value we set eax when calling cpuid)
    System::cpuid(0,&maxi,&ebx,&ecx,&edx); 
    Console::formatLine("cpuid level: %d", maxi);

    int stepping,model,family,type,reserved,brand,siblings;
    int apic_id;

    cpuid(1,&eax,&ebx,&unused,&edx);
    printf("version: %08lx\n",eax);

    stepping = eax & 0xf;
    printf("stepping: %d\n", stepping);
    model = (eax >> 4) & 0xf;
    printf("model: %d\n", model);
    family = (eax >> 8) & 0xf;
    printf("family: %d\n", family);
    type = (eax >> 12) & 0x3;
    
    // reserved = eax >> 14;
    //clf = (ebx >> 8) & 0xff;
    apic_id = (ebx >> 24) & 0xff;
    printf("current cpu id: %d\n", apic_id);
    siblings = (ebx >> 16) & 0xff;
    printf("logical: %d\n", siblings);
    //feature_flags = edx;
    
}


void dump2()
{
    int32 mask1 = 0;
    int32 mask2 = 0;
    for( unsigned int i = 0; i < 16; i += 1 )
    {
        mask1 = 1 << i;
        Console::writeLine(String::Bits(mask1));
        Console::formatLine("%08x", mask1);
        
        mask2 |= 1 << i;
        Console::writeLine(String::Bits(mask2));
        Console::formatLine("%08x", mask2);
        
        if (mask1 & mask2)
            Console::writeLine("  true ");
        else
            Console::writeLine("  false ");
        
    }
    Console::writeLine("------");
    int32 j = 1;
    mask2 = 1 << 2;
    mask1 = 1 << j;
    Console::writeLine(String::Bits(mask1));
    Console::writeLine(String::Bits(mask2));
    if (mask1 & mask2)
        Console::writeLine("  true ");
    else
        Console::writeLine("  false ");
        
    mask2 |= mask1;
    Console::writeLine(String::Bits(mask2));

    if (mask1 & mask2)
        Console::writeLine("  true ");
    else
        Console::writeLine("  false ");
    
}

void dump()
{
    uint32 maxi, eax, ecx, ebx, edx, unused;
    // get the max function value  (value we set eax when calling cpuid)
    System::cpuid(0,&maxi,&unused,&unused,&unused);
 
    //uint32 affinity_mask = System::GetProcessAffinity();
    //Console::writeLine(String::Bits((int32)affinity_mask).text());
 
     Console::formatLine("active cpu apic id: %x", CpuInfo::GetActiveApicId());
    //maxi &= 0xffff; /* The high-order word is non-zero on some Cyrix CPUs */
    printf(" eax in    eax      ebx      ecx      edx\n");
    for(int i=0;i<=maxi;i++)
    {
        cpuid(i,&eax,&ebx,&ecx,&edx);
        printf("%08x %08lx %08lx %08lx %08lx\n",i,eax,ebx,ecx,edx);
    }    
    
    System::cpuid(0x80000000,&maxi,&unused,&unused,&unused);
    for(uint32 li=0x80000000;li<=maxi;li++)
    {
        cpuid(li,&eax,&ebx,&ecx,&edx);
        printf("%08x %08lx %08lx %08lx %08lx\n",li,eax,ebx,ecx,edx);
    }
}

int main (int argc, char const* argv[])
{
    App::ArgumentParser argparser("SRL Network Tool", "This is an example of how to use SRL Net Logic", "0.0.0");
    argparser.add("verbose", "-v, --verbose", "display verbose output");
    argparser.add("help", "--help", "display this output");
	argparser.add("dump", "-d, --dump", "dump all cpuid results in hex");
	argparser.add("all", "-a, --all", "run command on each logical cpu");
	argparser.add("extended", "-e, --extended", "show extended info");

    argparser.parse(argc, argv);
     
    if (argparser.get("all")->wasParsed())
    {
        uint32 logical_cpus = System::GetCpuCount();
        for (int i = 0; i < logical_cpus; ++i)
        {
            CpuSet cpu_set;
            cpu_set.add(i);
            SetProcessAffinity(cpu_set);
            Console::formatLine("\n=== logical cpu index: %d ===", i);
            if (argparser.get("dump")->wasParsed())
                dump();
            else
                basic_info();
        }
    }
    else if (argparser.get("dump")->wasParsed())
    {
        dump();
    }
    else
    {
        basic_info();
    }
    return 1;
}