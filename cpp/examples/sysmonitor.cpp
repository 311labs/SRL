/**
 * File: sysmonitor.cpp
 * Project: srl
 * Created by: Ian Starnes on 2007-01-15.
 * 
 * This library is free software; you can redistribute it and/or                 
 * modify it under the terms of the GNU Lesser General Public                    
 * License as published by the Free Software Foundation; either                  
 * version 2.1 of the License, or (at your option) any later version.            
 *                                                                               
 * This library is distributed in the hope that it will be useful,               
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             
 * Lesser General Public License for more details.                               
 *                                                                               
 * You should have received a copy of the GNU Lesser General Public              
 * License along with this library; if not, write to the Free Software           
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.   
 */
#include "srl/Console.h"                          
#include "srl/sys/SystemInfo.h"
#include "srl/app/ArgumentParser.h"
#include "srl/fs/TextFile.h"
#include "srl/net/NetworkInterface.h"

using namespace SRL;
using namespace SRL::System;

#define MEGABYTE 1048576.0f
bool VERBOSE = false;

void log(const String& logfile, const String& ifacename, const int& refreshtime)
{
    SystemInfo *sysinfo = SystemInfo::GetInstance();
     
    // TODO catch sigint and close file
    FS::TextFile *tfile = FS::CreateTextFile(logfile, true);
    while (true)
    {
        System::Sleep(refreshtime);
        sysinfo->refresh();
        tfile->format("%0.2f, %0.2f, %0.2f, ", sysinfo->cpu().load(), sysinfo->cpu().userLoad(),
                sysinfo->cpu().kernelLoad());
        tfile->format("%u, %u, ", sysinfo->memory().total(), sysinfo->memory().used());
        tfile->format("%u, %u", sysinfo->swap().total(), sysinfo->swap().used());
        Net::NetworkInterface *iface = Net::NetworkInterface::GetByName(ifacename);
        if (iface != NULL)
        {
            tfile->format(", %ub, %up, %u, %u, %u, ", iface->rx().bytes(), iface->rx().packets(), 
                        iface->rx().multicast(), iface->rx().errors(), iface->rx().dropped());
            tfile->format("%ub, %up, %u, %u, %u", iface->tx().bytes(), iface->tx().packets(), 
                        iface->tx().multicast(), iface->tx().errors(), iface->tx().dropped());
        }
        tfile->write("\n", 1);
        tfile->commit();
    }
    tfile->close();
    
}

int main (int argc, char const* argv[])
{
    App::ArgumentParser argparser("SRL System Monitor", "This is an example of how to use SRL SystemInfo", "0.0.0");
    argparser.add("verbose", "-v, --verbose", "display verbose output");
    argparser.add("help", "--help", "display this output");
    argparser.add("refresh", "-r, --refresh", "refresh time in milliseconds", "1000");
    argparser.add("iface", "-i, --iface", "network interface to record io data on", "eth0");
    argparser.add("logfile", "-l, --logfile", "where to log output to", "sysinfo.log");
    argparser.parse(argc, argv);
     
    if (argparser.get("logfile")->wasParsed())
    {
        try
        {
            log(argparser.get("logfile")->getValue(), argparser.get("iface")->getValue(), 
                argparser.get("refresh")->getIntValue());
        }
        catch(SRL::Errors::Exception &e)
        {
            Console::writeLine(e.message().text());
            return 1;
        }
    }
    else
    {
        argparser.printHelp();
    }

    return 0;
}

