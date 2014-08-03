#ifndef _SYSTEMMONITOR_H_
#define _SYSTEMMONITOR_H_

#include "srl/app/Monitor.h"
#include "srl/sys/SystemInfo.h"
#include "srl/net/NetworkInterface.h"

namespace SRL
{
    namespace Radmin
    {
        struct SystemStats
        {
            System::SystemInfo::CPU cpu;
            System::SystemInfo::Memory memory;
            System::SystemInfo::Memory swap;
        };
        typedef Util::Vector<SystemStats> SystemHistory;

        // FIXME not the best way to store network interface history
        struct NetworkStats
        {
            uint32 interfaces;
            String names[4];
            Net::NetworkInterface::IO out[4];
            Net::NetworkInterface::IO in[4];
        };
        typedef Util::Vector<NetworkStats> NetworkHistory;
         
        /**
        * SystemMonitor
        * Keeps a running history of different system statistics
        * Also Triggers can be set to send notification on events
        */
        class SystemMonitor : public Monitor
        {
        public:
            /** default contstructor */
            SystemMonitor(bool sys_stats=true, bool net_stats=false, bool disk_stats=false);
            /** default destructor */
            virtual ~SystemMonitor();
            
            /** returns the history for the system stats */
            const SystemHistory& getSystemHistory() const{ return _sys_history; }
            /** returns the history for the system stats */
            const NetworkHistory& getNetworkHistory() const{ return _net_history; }
            /** returns the history for the system stats */
            //const DiskHistory& getDiskHistory() const{ return _disk_history; }            
        protected:
            bool run();
        
        protected:
            SystemHistory _sys_history;
            NetworkHistory _net_history;
            //DiskHistory _disk_history;
            
        };
        
    } /* App */
    
} /* SRL */


#endif /* _SYSTEMMONITOR_H_ */
