/*********************************************************************************
* Copyright (C) 1999,2006 by Ian C. Starnes   All Rights Reserved.        
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*******************************************************************************/

#ifndef __SRL_SYSTEM_INFO__
#define __SRL_SYSTEM_INFO__

#include "srl/sys/System.h"
#include "srl/Singleton.h"
#include "srl/util/Vector.h"

namespace SRL
{
    namespace System
    {
        /** Gathers System Information
        * CPU: Load, Avg Load, Total, Per Cpu
        *   TODO: Add CPU Temperature
        * Memory: Resident, Virtual
        *  TODO: Add Swap Pagein Pageout
        */
        class SRL_API SystemInfo : public Singleton<SystemInfo>
        {
        friend class Singleton<SystemInfo>;
        public:
        /** represents the operating system info */
            class SRL_API OS
            {
            friend class SystemInfo;
            public:
                /** name (Windows, Linux, Mac, FreeBSD) */
                const String& name() const { return _name; }
                /** version (95, ME, XP, NT, Gentoo, Redhat, OS X) */
                const String& version() const { return _version; }
                /** kernel (Darwin x.x.x, 2.6.9, SP5) */
                const String& kernel() const { return _kernel; }
                /** machine? */
                const String& machine() const { return _machine; }
            protected:
                String _name, _version, _build, _kernel, _machine;   
            };
            
            /** represents the system memory info (bytes) */
            class SRL_API Memory
            {
            friend class SystemInfo;
            public:
                /** returns the total memory */
                const uint32& total() const { return _total; }
                /** returns the amount of free memory */
                const uint32& free() const { return _free; }
                /** returns the amount of used memory */
                const uint32& used() const { return _used; }
            protected:
                Memory() : _used(0), _free(0), _total(0){}
                uint32 _used, _free, _total;                
            };
            
            /** represents the system cpu info */
            class SRL_API CPU
            {
            friend class SystemInfo;
            public:
                /** default destructor */
                virtual ~CPU(){};
                /** refresh the current cpu state */
                void refresh();
                /** compare our String to our String */
                friend bool operator==(const CPU& s1,const CPU& s2){ return (s1._number == s2._number); };
                /** processor index (0 = default) */
                const uint16& number() const { return _number; }
                /** returns the cpu freq in HZ */
                const float64& speed() const { return _freq; }
                /** vendor id */
                const String& vendor() const { return _vendor_id; }
                /** model name */
                const String& modelName() const { return _model_name; }
                
                /** family id */
                const uint32& family() const { return _family; }
                /** model id */
                const uint32& model() const { return _model; }
                /** type id */
                const uint32& type() const { return _type; }
                
                /** cache size (kb) */
                const uint32& cache() const { return _cache; }
                /** current cpu load */
                const float32& load() const { return _load; }
                /** current kernel load */
                const float32& kernelLoad() const{ return _kernel_load; }
                /** current user load */
                const float32& userLoad() const { return _user_load; }
            
                /** user time */
                const uint32& userTime() const { return _user; }
                /** kernel time */
                const uint32& kernelTime() const { return _kernel; }
                /** idle time */
                const uint32& idleTime() const { return _idle; }

                /** returns true if the cpu is hyperthreaded */
                //bool isHyperThreaded();
                // TODO add set cpu freq (if speed stepping is supported)
            protected:
                /** called by refresh to update the cpu timers kernel, user, idle */
                void update_times();
                /** construct the cpu instance */
                CPU(uint16 number, bool is_avg=false);
                /** refresh the cpu speed */
                void _refreshSpeed(const float64 &elapsed_time);
            
                //void init();
                Mutex _mutex;
                uint16 _number;
                float64 _freq;
                String _vendor_id;
                String _model_name;
                uint32 _cache;
                mutable uint32 _user, _kernel, _nice, _idle;
                mutable float32 _load, _user_load, _kernel_load;
                mutable float64 _last_update;
                mutable uint64 _last_clocks;
                bool _is_avg;
                SRL::byte _apic_id, _smt_id, _core_id, _package_id;
                uint32 _model, _family, _type;
            };
                        
            /** current hostname */
            const String& name() const { return _host_name; }
            /** current domain */
            const String& domain() const { return _domain; }        
                    
            /** OS Info */
            const OS& os() const { return _os; }
                    
            /** returns the current number of logical cpu's */
            const uint32& cpuCount() const{ return _cpus.size();};
            /** get cpu class intance by cpu physical id(index) */
            const CPU& getCPU(const uint32 &index) const { return *_cpus[index]; }
            /** returns the cpu object that represents the entire system */
            const CPU& cpu() const{ return *_cpu_avg; }
            
            /** checks if hyperthreading is supported */
            const bool& isHyperThreadSupported() const { return _ht_support; }
            /** checks if hperthreading is enabled */
            const bool& isHyperThreadEnabled() const { return _ht_enabled; }
            
            /** number of logical cpus */
            const uint32& numberOfLogicalCPUs() const { return _num_logical_cpus; }
            /** number of physical cpus (or cpu packages) */
            const uint32& numberOfPhysicalCPUs() const { return _num_physical_cpus; }
            /** number of cores per cpu(package) */
            const uint32& numberOfCoresPerCPU() const { return _cores_per_cpu; }
            
            /** returns the system memory object */
            const Memory& memory() const { return *_memory; }
            /** returns the swap memory object */
            const Memory& swap() const { return *_swap; }
            
            /** returns the current uptime for the system in seconds */
            const uint32& uptime() const { return _uptime; }
            /** returns the current uptime in the string format */
            const String& strUptime();
            
            /** refresh current info */
            void refresh();                 
        protected:
            SystemInfo();
            ~SystemInfo();
            
            void _update_os();
            void _update_cpus();
            void _update_cpu_info();
            void _update_memory();
            
            /** pointer to our cpu average */
            CPU *_cpu_avg;
            /** list of cpus */
            Util::Vector<SystemInfo::CPU*> _cpus;
            /** system memory */
            Memory* _memory;
            Memory* _swap;
            /** OS Info */
            OS _os;
            /** current host name */
            String _host_name;
            /** current domain name */
            String _domain;
            
            uint32 _num_logical_cpus, _num_physical_cpus, _cores_per_cpu;
            bool _ht_support, _ht_enabled;
            uint32 _uptime;
            String _str_uptime;
            float64 _last_update;
        };
    }   
}

#endif

