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

#ifndef _PROCESSLIST_H_
#define _PROCESSLIST_H_

#include "srl/Singleton.h"
#include "srl/NonCopyable.h"
#include "srl/DateTime.h"
#include "srl/util/Dictionary.h"
#include "srl/util/Vector.h"
#include "srl/fs/Directory.h"

namespace SRL
{
    namespace System
    {
        /** Process List 
        * The current running process list with process info(CPU, Memory, Threads, Args, etc)
        * TODO Make ProcessList a thread that runs with startListener(listener) is called
        */
        class SRL_API ProcessList : public Singleton<ProcessList>
        {
        friend class Singleton<ProcessList>;            
        public:
            /** ProcessList Process */
            class SRL_API Process : private NonCopyable
            {
            friend class ProcessList;
            public:
                /** Process CPU Info */
                class CPU
                {
                friend class Process;
                friend class ProcessList;
                public:
                    CPU(int cpu_count): _cpu_num(0), _cpu_count(cpu_count),
                     _user_time(0),_kernel_time(0),_last_timer(0.0),
                    _load(0.0f),_user_load(0.0f),_kernel_load(0.0f), _sys_load(0.0f){}
                    /** total cpu load for this process */
                    const float32& load() const { return _sys_load; }
                    /** total physical ram memory actually in use */
                    const float32& userLoad() const { return _user_load; }
                    /** total physical memory in use */
                    const float32& kernelLoad() const { return _kernel_load; }
                    /** time in user mode (jiffies 1/100th second) */
                    const uint32& userTime() const { return _user_time; }
                    /** time in kernel mode (jiffies 1/100th second) */
                    const uint32& kernelTime() const { return _user_time; }
                    /** cpiu number that this process execute last on */
                    const uint32& cpuNumber() const { return _cpu_num; }
                protected:
                    /** pass in the current user and kernel time in jiffies */
                    void update(const uint32& user_time, const uint32& kernel_time);
                    uint32 _cpu_num;
                private:
                    int _cpu_count;
                    uint32 _user_time, _kernel_time;
                    float64 _last_timer; 
                    float32 _load, _user_load, _kernel_load, _sys_load;                   
                    
                };
                
                /** Process Memory Info */
                class SRL_API Memory
                {
                friend class Process;
                friend class ProcessList;
                public:
                    Memory():_resident(0),_virtual(0), _faults(0), _pageins(0){}
                    /** total virtual memory allocated to process */
                    const uint32& residentSize() const { return _resident; }
                    /** total physical ram memory actually in use */
                    const uint32& virtualSize() const { return _virtual; }
                    /** number of page faults */
                    const uint32& faults() const { return _faults; }
                    /** number of page faults */
                    const uint32& pageins() const { return _pageins; }
                protected:
                    uint32 _resident, _virtual, _faults, _pageins;                    
                    
                };
  
                /** default constructor for an existing process */
                Process(const uint32& pid, int cpu_count);
                /** destructor */
                virtual ~Process();
                
                /** return the pid of the process */
                const uint32& id() const { return _pid;}
                /** return the current image name */
                const String& name() const { return _name;}
                /** returns the full path and file name */
                const String& filename() const { return _filename;}
                /** return the command used */
                const String& command() const { return _command;}
                /** return the owner of the process */
                const String& owner() const { return _owner;}
                /** return the parent id (0 for none) */
                const uint32& parentId() const { return _parent_id; }
                /** return the parent process (null if has none) */
                //Process* parent();
                /** reference to process cpu object */
                const CPU& cpu() const { return _cpu; }
                /** reference to process memory object */
                const Memory& memory() const { return _memory; }
                
                /** number of threads owned by this process */
                const uint32& threads() const { return _threads; }
                /** number of handles held by this process */
                const uint32& handles() const { return _handles; }
                /** current priority level */
                const uint32& priority() const { return _priority; }
                /** modules loaded */
                // future a list of modules (libraries,etc.)
            
                /** return the datetime the process was created */
                const DateTime& created() const { return _created; }
            
                /** is running */
                const bool& isRunning() const {return _is_running; }
            
                /** update the process information */
                void refresh();
            
                /** terminate the current process */
                bool terminate(bool force=false);                
                
            protected:
                void initialize();
                void cleanup();
                // pid
                uint32 _pid;
                // thread count, handle count
                uint32 _threads, _handles;
                // memory
                Memory _memory;
                // cpu load
                CPU _cpu;
                // process name
                String _name;
                // process owner
                String _owner;
                // process owner domain
                String _domain;
                // process command line
                String _command;
                // full path 
                String _filename;
                // date created
                DateTime _created;
                // current priority level
                uint32 _priority;
    #ifdef linux
                FS::Directory *_fd_directory;
    #elif defined(OSX)
                char* _mac_args;
    #endif
                // misc state variables
                uint32 _uid, _gid;
                uint32 _parent_id;
                int32 _tty;
                bool _is_running, _is_new;                
                
            };

            /** ProcessList Listener Interface */
            class Listener
            {
            public:
                Listener(){};
                virtual ~Listener(){};
                /** called when a new process is started */
                virtual void event_process_started(const Process& process)=0;
                /** called when a process a process has terminated */
                virtual void event_process_stopped(const Process& process)=0;
            };

            /** update the list of processes and their state */
            void refresh();            

            /** ProcessList Iterator */
            typedef Util::Dictionary<uint32, Process*>::Iterator Iterator;
            /** return the first process iterator in the list */
            Iterator begin() const{ return _list.begin(); }
            /** return the end of list iterator(does not actually point to a process) */
            Iterator end() const{ return _list.end(); }
            
            /** get a process by its pid */
            Process* getProcess(const uint32& pid) const;            
            /** get a process by its name */
            Process* getProcess(const String& name, bool ignore_case=false) const;
            /** return the number of processes */
            const uint32& size() const { return _list.size(); }

            /** returns a vector of pids that contains all new processes since last update */
            Util::Vector<uint32>& newPids(){ return _new_pids; }
            /** returns a vector of pids that contains all dead processes since last update */
            Util::Vector<uint32>& deadPids(){ return _dead_pids; }
            
        protected:
            /** Default Constructor Returns a list of current running processes */
            ProcessList();  
            /** default destructor */
            virtual ~ProcessList();
            
            /** list of processes */
            Util::Dictionary<uint32, Process*> _list;
            /** listener that receives new process events */
            Listener *_listener;
            /** the last update */
            float64 _last_update;
            
        private:
            /** removes dead processes */
            void remove_dead();
            /** list of active pids */
            Util::Vector<uint32> _pids;         
            /** list of new pids since last update */
            Util::Vector<uint32> _new_pids;
            /** list of dead pids since last update */
            Util::Vector<uint32> _dead_pids;
            /** number of cpus */
            int _cpu_count;
        };
     
        
    }
}



#endif /* _PROCESSLIST_H_ */


