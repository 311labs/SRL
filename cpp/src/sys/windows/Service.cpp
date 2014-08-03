#include "srl/sys/Daemon.h"
#include "srl/log/LogFile.h"
#include "srl/log/SysLog.h"

using namespace SRL;
using namespace SRL::System;

Daemon *Daemon::__instance = NULL;

void Daemon::ServiceMain(DWORD argc, LPSTR *argv)
{
    try
    {
        Daemon::__instance->_initialize();
		Daemon::__instance->_is_running = true;
        while (Daemon::__instance->_keep_running && Daemon::__instance->run())
        {
			System::Sleep(500);
        }        
    }
    catch (SRL::Errors::Exception &e)
    {
		Daemon::__instance->thread_exception(e);
    }
    Daemon::__instance->final();   
    
	Daemon::__instance->_service_status.dwWin32ExitCode = 0;
    Daemon::__instance->_service_status.dwCurrentState       = SERVICE_STOPPED; 
    Daemon::__instance->_service_status.dwCheckPoint         = 0; 
    Daemon::__instance->_service_status.dwWaitHint           = 0;
    
    if (!::SetServiceStatus (Daemon::__instance->_service_status_handle, &Daemon::__instance->_service_status)) 
	{
		Daemon::__instance->_log.error(System::GetLastErrorString());
	}
	Daemon::__instance->_is_running = false;
}

void Daemon::ControlHandler(DWORD control)
{
    switch (control)
    {
       case SERVICE_CONTROL_PAUSE :
       case SERVICE_CONTROL_CONTINUE :
       case SERVICE_CONTROL_INTERROGATE :
		   break;
	   case SERVICE_CONTROL_SHUTDOWN:
       case SERVICE_CONTROL_STOP :
		   Daemon::__instance->_keep_running = false;
		   while (Daemon::__instance->isRunning())
		   {
			   System::Sleep(1000);
		   }
           break;
    }
}

Daemon::Daemon(const String& name) :
_log(Log::Logger::GetInstance(name)),
_name(name),
_path(),
_log_path(),
_user(),
_group(),
_keep_running(true),
_is_running(false)
{
    if (Daemon::__instance != NULL)
    {
        throw SRL::Errors::ServiceException("Service Instance Already Created");
    }
    Daemon::__instance = this;
}

Daemon::~Daemon()
{
    
}

bool Daemon::isRunning()
{
    return _is_running;
}

void Daemon::install(const String& prog_path)
{
    // const String& path, const String& name, const String& display_name, const String& username, 
    // const String& password, const String& domain
    Windows::Service::Install(prog_path, _name, _name, _user, "", "");
}

void Daemon::uninstall()
{
    SC_HANDLE h_service;
    Windows::Service::Open(_name, h_service);
    Windows::Service::Delete(h_service);
    Windows::Service::Close(h_service);
}

bool Daemon::start()
{
    SERVICE_TABLE_ENTRY DispatchTable[]={{_name._strPtr(),Daemon::ServiceMain},{NULL,NULL}};  
    ::StartServiceCtrlDispatcher(DispatchTable);
	return true;
}

void Daemon::_initialize()
{
    _service_status.dwServiceType        = SERVICE_WIN32; 
    _service_status.dwCurrentState       = SERVICE_START_PENDING; 
    _service_status.dwControlsAccepted   = SERVICE_ACCEPT_STOP; 
    _service_status.dwWin32ExitCode      = 0; 
    _service_status.dwServiceSpecificExitCode = 0; 
    _service_status.dwCheckPoint         = 0; 
    _service_status.dwWaitHint           = 0;
    
    _service_status_handle = ::RegisterServiceCtrlHandler(_name.text(), Daemon::ControlHandler); 
    if (_service_status_handle == (SERVICE_STATUS_HANDLE)0) 
        throw SRL::Errors::ServiceException(System::GetLastErrorString());
    
    this->initial();
    
    _service_status.dwCurrentState       = SERVICE_RUNNING; 
    _service_status.dwCheckPoint         = 0; 
    _service_status.dwWaitHint           = 0;
    
    if (!::SetServiceStatus (_service_status_handle, &_service_status)) 
        throw SRL::Errors::ServiceException(System::GetLastErrorString());
}

void Daemon::runForever()
{
    try
    {
		_is_running = true;
		if (initial())
        {
            while (_keep_running && run())
            {
                System::Sleep(500);
            }

        }
    }
    catch (SRL::Errors::Exception &e)
    {
        _log.error(e);
    }	        
    
	final();
	_is_running = false;
}

bool Daemon::stop()
{
    SC_HANDLE h_service;
    Windows::Service::Open(_name, h_service);
    Windows::Service::Stop(h_service);
    Windows::Service::Close(h_service);
	return true;
}

bool Daemon::reload()
{
	return false;
}

