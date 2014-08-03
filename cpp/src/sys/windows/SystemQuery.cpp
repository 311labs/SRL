
#include "srl/sys/SystemQuery.h"

#include "srl/Exceptions.h"

#include <tchar.h>

using namespace SRL;
using namespace SRL::System;

uint32 System::QuerySystemInfo(uint32 info_class, void* buffer,
                               uint32 buffer_size, uint32* req_size)
{
    // handle to the ntdll.dll
    static HINSTANCE ntdll_handle = NULL;
    // pointer to the method we want in the ntdll.dll
    static uint32 (WINAPI * ZwQuerySystemInformation)(uint32, void*, uint32, uint32*);

    // check if we already loaded the dll
    if (ntdll_handle == NULL)
    {
        // get the NTDLL.dll handle
        if ((ntdll_handle = ::GetModuleHandle(_T("ntdll.dll"))) == NULL)
        {
			throw Errors::LogicException("could not load ntdll.dll");
        }
        
        // map the ZwQuerySystemInformation method
        *(FARPROC*)&ZwQuerySystemInformation = ::GetProcAddress(ntdll_handle, "ZwQuerySystemInformation");
        if (ZwQuerySystemInformation == NULL)
        {
            ntdll_handle = NULL;
            throw Errors::LogicException("failed to find ZwQuerySystemInformation in ntdll.dll");
        }
    }
    return ZwQuerySystemInformation(info_class, buffer, buffer_size, req_size);
}

SystemQuery::SystemQuery(SystemQueryType query_type):buffer(NULL),buffer_size(0x8000)
{
    heap_handle = ::GetProcessHeap();
        // now we will loop until our buffer is big enough
    uint32 status, needed;
    needed = 0;
    do
    {
        buffer = ::HeapAlloc(heap_handle, 0, buffer_size);
        if (buffer == NULL)
            throw Errors::LogicException("not enough memory");
        
        status = QuerySystemInfo(query_type, buffer, buffer_size, &needed);
        
        if (status == STATUS_INFO_LENGTH_MISMATCH)
        {
            ::HeapFree(heap_handle, 0, buffer);
            buffer_size = needed;
        }
        else if (!NT_SUCCESS(status))
        {
            ::HeapFree(heap_handle, 0, buffer);
            throw Errors::LogicException("call failed to QuerySystemInfo");
        }
    
    } while (status == STATUS_INFO_LENGTH_MISMATCH);
}

SystemQuery::~SystemQuery()
{
    ::HeapFree(heap_handle, 0, buffer);
}
