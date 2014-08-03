#ifndef _SRL_MONITOREDPROCESS_
#define _SRL_MONITOREDPROCESS_

#include "srl/sys/Process.h"
#include "srl/radmin/Monitor.h"

namespace SRL
{
    namespace Radmin
    {
        /**
        * MonitoredProcess
        * This is a way to spawn a process that is then monitored by a thread
        * for certain events that may occur.
        * 
        */
        class MonitoredProcess : public Monitor
        {
        public:
            /** default contstructor */
            MonitoredProcess(const String& command, System::UserInfo &user);
            MonitoredProcess(const String& command);
            /** default destructor */
            virtual ~MonitoredProcess();
            
            /** set the path where we should create out log file */
            void setLogPath(const String& logpath);
            
            /** returns the pid for the monitored process */
            const int& pid() const{ return _pid; }
            
            /** returns an rpc struct with the current info for the monitored process, caller is responsible for cleanup */
            RPC::RpcStructValue* asRpcValue() const;
            
        protected:
            bool initial();
            bool run();
            void final();
            void thread_exception(SRL::Errors::Exception &e);
            
        protected:
            String _cmd;
            String _log_path;
            System::UserInfo _user;
            System::Process* _process;
            int _pid;
        };
        
    } /* App */
    
} /* SRL */


#endif /* _SRL_MONITOREDPROCESS_ */
