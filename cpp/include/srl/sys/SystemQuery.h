#ifndef __SRL_SystemQuery__
#define __SRL_SystemQuery__

#include "srl/sys/System.h"

namespace SRL
{
	namespace System
	{

#define HAS_QUERYSYSTEM 1
#define NT_SUCCESS(Status) ((uint32)(Status) >= 0)
#define STATUS_INFO_LENGTH_MISMATCH      ((uint32)0xC0000004L)

#define FILETIME_TO_MICROSECONDS(a) (uint32)(*((int64*) &(a)))/ 10
#define FILETIME_TO_JIFFIES(a)  (uint32)(*((int64*) &(a)))/ 100000


// lets define the structures needed for ZwQuerySystemInformation
struct SYSTEM_LOAD_INFORMATION
{
    int64 idle_time;
    int64 kernel_time;
    int64 user_time;
    int64 reserved_1[2];
    uint32 reserved_2;
};

struct SYSTEM_TIME_INFORMATION
{
    int64 boot_time;
    int64 sys_time;
    int64 timezone_bias;
    uint32 timezone;
    uint32 reserved;
};

struct CLIENT_ID
{
    uint32 unique_process;
    uint32 unique_thread;
};

struct UNICODE_STRING
{
    uint16 length;
    uint16 max_length;
    PWSTR buffer;
};

struct VM_COUNTERS
{
    SIZE_T peak_virtual_size;
    SIZE_T virtual_size;
    uint32 page_fault_count;
    SIZE_T peak_working_set_size;
    SIZE_T working_set_size;
    SIZE_T quota_peak_paged_pool_usage;
    SIZE_T quota_paged_pool_usage;
    SIZE_T quota_peak_non_paged_pool_usage;
    SIZE_T quota_non_paged_pool_usage;
    SIZE_T pagefile_usage;
    SIZE_T peak_pagefile_usage;
};

struct SYSTEM_THREAD_INFORMATION
{
    FILETIME kernel_time;
    FILETIME user_time;
    FILETIME create_time;
    uint32 wait_time;
    void* start_address;
    CLIENT_ID client_id;
    uint32 priority;
    uint32 base_priority;
    uint32 context_switch_count;
    int32 state;
    int32 wait_reason;
};

struct SYSTEM_PROCESS_INFORMATION
{
    uint32 next_entry_delta;
    uint32 thread_count;
    uint32 reserved1[6];
    FILETIME create_time;
    FILETIME user_time;
    FILETIME kernel_time;
    UNICODE_STRING name;
    uint32 base_priority;
#ifdef WIN64
    uint64 id;
    uint64 parent_id;
#else
    uint32 id;
    uint32 parent_id;
#endif
	uint32 handle_count;
    uint32 reserved2[2];
    VM_COUNTERS vm_counters;
#if _WIN32_WINNT >= 0x500
    IO_COUNTERS io_counters;
#endif
    SYSTEM_THREAD_INFORMATION threads[1];
};

enum SystemQueryType
{
    QUERY_BASIC_INFO=0,
    QUERY_SYS_TIME=3,
    QUERY_PROCESS_INFO=5,
    QUERY_CPU_LOAD=8
};

uint32 QuerySystemInfo(uint32 info_class, void* buffer,
                               uint32 buffer_size, uint32* req_size);

struct SystemQuery
{
    HANDLE heap_handle;
    uint32 buffer_size;
    void* buffer;

    SystemQuery(SystemQueryType query_type);
    virtual ~SystemQuery();
};

}
}

#endif


