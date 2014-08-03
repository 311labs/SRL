#ifndef _SRL_RADMIN_
#define _SRL_RADMIN_

#include "srl/sys/Daemon.h"
#include "srl/radmin/Monitor.h"
#include "srl/xml/XmlRpcProtocol.h"
#include "srl/app/IniParser.h"
#include "srl/log/Logger.h"
#include "srl/radmin/RadminFTP.h"

namespace SRL
{
    namespace Radmin
    {
        
        /**
        * Simple thread to handle monitor cleanup 
        */
        class GarbageCollector : public System::Thread
        {
        public:
            GarbageCollector();
            virtual ~GarbageCollector();
            void add(Monitor *monitor);
            void add(RadminFTP *ftp);
        protected:
            System::Mutex _critsec;
            Util::Vector<Monitor*> _garbage;
            Util::Vector<RadminFTP*> _ftp_garbage;
            bool run();
        };        
        
        /**
        * RadminService : public System::Daemon
        * The radmin serivce manages protocols and different monitors
        * that are loaded via a config file
        */
        class RadminDaemon : public System::Daemon, public Monitor::Listener, public RPC::RpcService
        {
        public:

            
            /** default contstructor */
            RadminDaemon(const String& config_path);
            /** default destructor */
            virtual ~RadminDaemon();
            
            /** 
            * Starts a Monitored Process 
            * The arguments should be: 
            *   commands + argumetns as a string
            *   array of trigger stuctures
            *   option uid and gid
            */
            RPC::RpcValue* monitorProcess(RPC::RpcArrayValue *args);
            /**
            * Kill all the processes owned by the specified user
            */
            RPC::RpcValue* killAllProcs(const String& user);
        protected:
            // daemon callbacks
            bool initial();
            bool run();
            void final();
			void thread_exception(SRL::Errors::Exception &e);
            void stop_event();
            void reload_event();            
            // RPC Callback
            RPC::RpcValue* execute(const String& method_name, RPC::RpcArrayValue *args);
            
            // monitor call backs
            void monitor_triggered(Monitor &monitor, Trigger &trigger);
            void monitor_error(Monitor &monitor, SRL::Errors::Exception &e);
            void monitor_stopped(Monitor &monitor);
            
            void createEventChannel();
            
            int getNextFtpPort();
            
        protected:
            Util::Vector<Monitor*> _monitors;
            Util::Vector<RadminFTP*> _ftp_servers;
            Net::Socket _event_channel;
            App::IniParser _config;
            XML::XmlRpcProtocol *_protocol;
            Net::HttpServer *_http_server;
            GarbageCollector _gc;
            int _ftp_port;
            System::Mutex _event_critsec;
            System::Mutex _ftp_critsec;
        };
        
        
    } /* Radmin */
    
} /* SRL */


#endif /* _SRL_RADMIN_ */
