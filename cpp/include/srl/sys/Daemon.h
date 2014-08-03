/**
*  Daemon
*
*  Created by Ian Starnes on 2006-09-19.
*  Copyright (c) 2006. All rights reserved.
*
******************************************************************************
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) any later version.          *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public            *
* License along with this library; if not, write to the Free Software         *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.  *
******************************************************************************/

#ifndef __SRL_Daemon__
#define __SRL_Daemon__

#include "srl/log/Logger.h"
#include "srl/sys/Thread.h"

#ifdef WIN32
#include "srl/win/Service.h"
#endif

namespace SRL
{
    
    namespace Errors
    {
        /** Thrown own a service logic error.  */
        class ServiceException : public Exception
        {
        public:
            /** default constructor */
            ServiceException(String message): Exception(message){};
        };  
        /** Thrown when the thread appears to be deadlocked */
        class ServiceAlreadyStarted : public ServiceException
        {
        public:
            /** default constructor */
            ServiceAlreadyStarted(String message): ServiceException(message){};
        };  
        
        /** Thrown when the thread appears to be deadlocked */
        class ServiceLogicTimedOut : public ServiceException
        {
        public:
            /** default constructor */
            ServiceLogicTimedOut(String message): ServiceException(message){};
        };        
        
    }    
    
    
    namespace System
    {
        
        /** This class wraps up Windows Daemons and UNIX Daemons 
        * Calling start will fork off the service and return the current thread
        */
    	class SRL_API Daemon : public Runnable
    	{
        public:
            /** create a service with the given name */
            Daemon(const String& name);
            /** destructor */
            virtual ~Daemon();
            
            /** 
            * set the user the and group name that the process should run as 
            * if the group or user is empty then that field is ignored
            */
            void setOwner(const String& user, const String& group){ _user = user; _group = group; }
            /** set the working directory */
            void setWorkingPath(const String& path){ _path = path; }
            /** 
            * set log path 
            * for a SystemLog set the path to SYSLOG for standard out set it to empty string
            */
            void setLogPath(const String& path){ _log_path = path; }
    
            /** returns the name of the service */
            const String& getName() const{return _name;}
            /** returns the pid of the service */
            const int& getPid() const{return _pid;}
            /** returns true if the service is currently running */
            bool isRunning();
        
            /** this is currently only used for windows */
            void install(const String& prog_path);
            /** this is currently only used for windows */
            void uninstall();
        
            /** this will start the service as another process */
            bool start();
            /** runForever() will run the service in the calling threads process */
            void runForever();
            
            /** this will attempt to find the pid and stop the service by name */
            bool stop();
            /** reload the service */
            bool reload(); 

        protected:
            static Daemon* __instance;
#ifdef WIN32
            static void WINAPI ServiceMain(DWORD argc, LPSTR *argv);
			static void WINAPI ControlHandler(DWORD control);
            SERVICE_STATUS _service_status;
            SERVICE_STATUS_HANDLE _service_status_handle;
#else
            static void __signal_handler(int sig);
#endif
        protected:

            virtual bool initial(){return true;}
            virtual bool run() = 0;
            virtual void final(){return;}
            virtual void thread_exception(SRL::Errors::Exception &e){ throw e; }
            virtual void stop_event()=0;
            virtual void reload_event()=0;

            void _initialize();

            /** local log pointer */
            SRL::Log::Logger& _log; 
            bool _keep_running, _is_running;
        private:
            String _name, _path, _log_path;
            String _user, _group;
            String _pid_file;
            int _pid;
        };
    }
}

#endif // __SRL_Daemon__

