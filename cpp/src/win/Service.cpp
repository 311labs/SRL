#include "srl/win/Service.h"
#include "srl/sys/Daemon.h"

using namespace SRL;
using namespace SRL::Windows;

void Service::Open(const String& name, SC_HANDLE &h_service)
{
    SC_HANDLE h_manager = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if (h_manager == NULL)
        throw SRL::Errors::ServiceException(System::GetLastErrorString());
    h_service = ::OpenService(h_manager, name.text(), SERVICE_ALL_ACCESS);
    if (h_service == NULL)
    {
        ::CloseServiceHandle(h_manager);
        throw SRL::Errors::ServiceException(System::GetLastErrorString());
    }
}

void Service::Close(SC_HANDLE &h_service)
{
    ::CloseServiceHandle(h_service);
}

void Service::Install(const String& path, const String& name, const String& display_name, const String& username, 
    const String& password, const String& domain)
{
    SC_HANDLE h_manager, h_service;
    h_manager = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);  
 
    if (h_manager == NULL) 
        throw SRL::Errors::ServiceException(System::GetLastErrorString());

    LPCTSTR lpszBinaryPathName=path.text();
    String user;
    
	if ((username.length() > 2) &&(username != "local system"))
    {
        if (domain == "local")
        {
            user = ".\\";
        }
        else
        {
            user = domain + "\\";
        }
        user += username;
        h_service = ::CreateService(h_manager,
            name.text(),               // name of the service 
            display_name.text(),       // service name to display 
            SERVICE_ALL_ACCESS,        // desired access 
            SERVICE_WIN32_OWN_PROCESS, // service type 
            SERVICE_DEMAND_START,      // start type 
            SERVICE_ERROR_NORMAL,      // error control type 
            lpszBinaryPathName,        // service's binary 
            NULL,                      // no load ordering group 
            NULL,                      // no tag identifier 
            NULL,                      // no dependencies 
            user.text(),               // administrator needed for Win2003 
            password.text());          // passwd required
    }
    else
    {
        h_service = ::CreateService(h_manager,
			name.text(),               // name of the service 
            display_name.text(),       // service name to display 
            SERVICE_ALL_ACCESS,        // desired access 
            SERVICE_WIN32_OWN_PROCESS, // service type 
            SERVICE_DEMAND_START,      // start type 
            SERVICE_ERROR_NORMAL,      // error control type 
            lpszBinaryPathName,        // service's binary 
            NULL,                      // no load ordering group 
            NULL,                      // no tag identifier 
            NULL,                      // no dependencies 
            NULL,                      // local system user
            NULL);                     // local system user
    }
 
    if (h_service == NULL) 
        throw SRL::Errors::ServiceException(System::GetLastErrorString());
 
    ::CloseServiceHandle(h_service); 

}

void Service::Delete(SC_HANDLE& h_service)
{
    if(::DeleteService(h_service)==0)
        throw SRL::Errors::ServiceException(System::GetLastErrorString());
}

void Service::Start(SC_HANDLE& h_service)
{
    if (!::StartService(h_service, 0, NULL))
        throw SRL::Errors::ServiceException(System::GetLastErrorString());
	SERVICE_STATUS_PROCESS ssp;
    Service::WaitForStatus(h_service, ssp, SERVICE_RUNNING);
}

void Service::Stop(SC_HANDLE& h_service)
{
    SERVICE_STATUS_PROCESS  ssp;
    
    Service::StopDependentServices(h_service);
    
    if (!::ControlService(h_service, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &ssp))
        throw SRL::Errors::ServiceException(System::GetLastErrorString());

    Service::WaitForStatus(h_service, ssp, SERVICE_STOPPED);
}

void Service::WaitForStatus(SC_HANDLE& h_service, SERVICE_STATUS_PROCESS& ssp, DWORD status_code)
{
    DWORD dwStartTime = GetTickCount();
    DWORD dwTimeout = 30000; // 30-second time-out
    
    while ( ssp.dwCurrentState != status_code ) 
    {
        
        System::Sleep( ssp.dwWaitHint );
        Service::QueryStatus(h_service, ssp);

        if ( GetTickCount() - dwStartTime > dwTimeout )
            throw SRL::Errors::ServiceLogicTimedOut("Timed out while waiting for service status to change");
    }
}

void Service::QueryStatus(SC_HANDLE& h_service, SERVICE_STATUS_PROCESS &ssp)
{
    DWORD dwBytesNeeded;
    
    if (!::QueryServiceStatusEx(h_service, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, 
                    sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
        throw SRL::Errors::ServiceException(System::GetLastErrorString());
}

void Service::StopDependentServices(SC_HANDLE& h_service)
{
    SC_HANDLE h_manager = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
     if (h_manager == NULL)
         throw SRL::Errors::ServiceException(System::GetLastErrorString());
   
    LPENUM_SERVICE_STATUS lp_dependencies = NULL;
    DWORD bytes_needed, count;
    
    // pass in 0 length buffer to probe for list, if the call succeeds then we have no dependencies
    if (::EnumDependentServices(h_service, SERVICE_ACTIVE, lp_dependencies, 0, &bytes_needed, &count))
        return;
    
    ENUM_SERVICE_STATUS ess;
    SC_HANDLE h_dep_service;
    SERVICE_STATUS_PROCESS ssp;
    
    lp_dependencies = (LPENUM_SERVICE_STATUS)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bytes_needed);
    if (lp_dependencies == NULL)
        throw SRL::Errors::ServiceException(System::GetLastErrorString());
        
    if (!::EnumDependentServices(h_service, SERVICE_ACTIVE, lp_dependencies, 0, &bytes_needed, &count))
    {
        String last_err = System::GetLastErrorString();
        ::HeapFree( GetProcessHeap(), 0, lp_dependencies);
        throw SRL::Errors::ServiceException(last_err);
    }
    
    for (DWORD i = 0; i < count; ++i) 
    {
        ess = *(lp_dependencies + i);
        h_dep_service = ::OpenService(h_manager, ess.lpServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
        if (h_dep_service == NULL)
        {
            String last_err = System::GetLastErrorString();
            ::HeapFree( GetProcessHeap(), 0, lp_dependencies);
            throw SRL::Errors::ServiceException(last_err);
        }
    
        Service::Stop(h_dep_service);
    }
    ::HeapFree( GetProcessHeap(), 0, lp_dependencies);   
}
