/*
* (c) Copyright 2009 Ian Starnes. All Rights Reserved. 
* 
* Service.h is part of 
*  is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  
* @author Ian Starnes <ian@starnes.de>
* @date August 02 2009  
*/
#include "srl/String.h"
#include <windows.h>
#include <winsvc.h>



namespace SRL
{
    namespace Windows
    {
        namespace Service
        {
            /**
            * Open a Handle to a Window Service
            * @param name   the name of the service to open
            * @param h_service  returned handle to the service
            */
            extern SRL_API void Open(const String& name, SC_HANDLE &h_service);

            /**
            * Close a Handle to a Window Service
            * @param h_service  the handle to the service
            */
            extern SRL_API void Close(SC_HANDLE &h_service);
            
            /**
            * Install a Service
            * @path     full path to the executable file
            * @name     the name of the service to be installed
            * @display_name     the display name for the service
            * @username     the username the service should run under
            * @password     the password for the user
            * @domain       the user domain
            */
            extern SRL_API void Install(const String& path, const String& name, const String& display_name, const String& username, 
                const String& password, const String& domain);
            
            /**
            * Delete an Installedd Service
            * @param h_service      handle to the service to delete
            */
            extern SRL_API void Delete(SC_HANDLE& h_service);
            
            /**
            * Start the service (will block until service is running)
            * @param h_service      handle to the service to start
            */
            extern SRL_API void Start(SC_HANDLE& h_service);
            
            /**
            * Stop the service (will block until service is stopped)
            * @param h_service      handle to the service to stop
            */
            extern SRL_API void Stop(SC_HANDLE& h_service);            
            
            /**
            * Stop the services dependents
            * @param h_service      handle to the parent service
            */
            extern SRL_API void StopDependentServices(SC_HANDLE& h_service);
            
            /**
            * Query the status of a service
            * @param h_service      handle of the service to query
            * @param status         the returned status
            */
            extern SRL_API void QueryStatus(SC_HANDLE& h_service, SERVICE_STATUS_PROCESS &status);
            
            /**
            * Waits for the service to change to the specified status
            * @param h_service          handle to the service to check
            * @param status_process     returned with the current status object
            * @param status_code        the status code to wait for 
            */
            extern SRL_API void WaitForStatus(SC_HANDLE& h_service, SERVICE_STATUS_PROCESS& ssp, DWORD status_code);
            
            
        } /* Service */ 
        
    } /* Windows */ 
    
} /* SRL */ 
