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
#ifndef __SRL_Process__
#define __SRL_Process__

#include "srl/io/TextReader.h"
#include "srl/io/TextWriter.h"
#include "srl/util/Vector.h"
#include "srl/sys/System.h"

namespace SRL
{
    namespace System
    {
        
        /** Class that spawns a child process and gives access to the processes stdout, stderr, and stdin 
        *TODO: look into getrusage() for processinfo updating
        */
        class SRL_API Process
        {
        public:
            /** default contructor
            *@param command     command to executed
            *@param uid         user id process should run as
            *@param mixin       combine error output with standard output
            *@param cpu_index   cpu affinity, if you want this to only run on one cpu(-1,0,1,2,3) default all=-1
            */
            Process(const String& command, uint32 uid=0, uint32 gid=0, bool mixin=true, int32 cpu_index=-1);
            /** default contructor
            *@param command     command to executed
            *@param path   sets the path the process will use as its working path   
            *@param uid         user id process should run as
            *@param mixin       combine error output with standard output
            *@param cpu_index   cpu affinity, if you want this to only run on one cpu(-1,0,1,2,3) default all=-1
            */
            Process(const String& command, const String& path, uint32 uid=0, uint32 gid=0, bool mixin=true, int32 cpu_index=-1);
            /** 
            * exiecute a command using the userinfo 
            * 
            */
            Process(const String& command, System::UserInfo &user_info, bool mixin=true, int32 cpu_index=-1);
            /** destructor */
            virtual ~Process();
            
            /** terminates the child process */
            void terminate();
            /** causes the calling thread to wait for the child process to terminate 
            *@returns the exit value of the process
            */
            int waitFor();
            
            /** is the child process still running */
            bool isRunning();
            
            /** returns the pid of the process */
            const int& pid() const{ return _pid;}    
            /** returns the exit value if the process has terminated */
            const int& exitValue() const{ return _exit_value;}
            /** returns the file image we executing */
            const String& file() const{ return _file;}
            
            /** returns the child processes standard output reader object */
            IO::TextReader& output(){ return *_output;}
            /** returns the child processes standard input writer object */
            IO::TextWriter& input(){ return *_input;}
            /** returns the child processes error output reader object */
            IO::TextReader& error(){ return *_error;}
            

        protected:
            /** initialize */
            void _initialize(const String& command);
            /** this is called by the constructor */
            void executeChild();
            /** pid of our child process */
            int _pid;
            /** used id we want the process to run as */
            System::UserInfo _user_info;
            /** index of the cpu to run on */
            int32 _cpu_index;
            /** flag to combine stdout and stderr */
            bool _mixin;
            /** the file we are executing */
            String _file;
            /** number of arguments */
            int _argc;
            /** exit value for our child process */
            int _exit_value;
            /** input writer object */
            IO::TextWriter *_input;
            /** output reader object */
            IO::TextReader *_output;
            /** error reader object */
            IO::TextReader *_error;

#ifdef WIN32
            HANDLE _hproc;
            String _command_line;
#else
            /** the argumentlist */
            Util::Vector<char*> _argv;            
#endif

            
        };
	}
}
#endif
