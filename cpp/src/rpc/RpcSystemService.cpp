#include "srl/rpc/RpcSystemService.h"
#include "srl/sys/SystemInfo.h"
#include "srl/sys/ProcessList.h"
#include "srl/net/NetworkInterface.h"
#include "srl/fs/TextFile.h"

using namespace SRL;
using namespace SRL::RPC;
using namespace SRL::Net;

#define BYTES_TO_MEGABYTES(b) b/1048576

RpcSystemService::RpcSystemService() :
RpcService("system")
{
    // now lets add all of the methods this service supports
    // we add them as null members because the service handles them directly
    // FIXME, this will have no help
    add("getInfo", NULL);
    add("getUsage", NULL);
    add("getNetUsage", NULL);
    add("configureNetwork", NULL);    
    add("getHistory", NULL);
    add("listProcesses", NULL);
    add("execute", NULL);
	add("getCommandOutput", NULL);
    add("getStatusOutput", NULL);
    add("runCommands", NULL);
    add("reboot", NULL),
    add("version", NULL);
    add("findProcess", NULL);
    add("getProcess", NULL);
    add("killProcess", NULL);
    add("pathExists", NULL);
    add("writeToFile", NULL);
    add("catFile", NULL);
    add("makeDir", NULL);
    add("removeDir", NULL);
}

RpcValue* RpcSystemService::execute(const String& method_name, RpcArrayValue *args)
{
    if (method_name == "getInfo")
        return getInfo();
    else if (method_name == "getUsage")
    {
        return getUsage();
    }
    else if (method_name == "getNetUsage")
    {
        return getNetUsage();
    }
    else if (method_name == "configureNetwork")
    {
        return configureNetwork(args);
    }    
    else if (method_name == "getHistory")
    {
        // TODO Add System History 
        return getHistory(10);    
    }
    else if (method_name == "listProcesses")
    {
        if ((args->size() > 0) && (args->get(0)->getType() == RpcValue::TypeString))
        {
            return listProcesses(args->get(0)->asString());
        }
        else
        {
            return listProcesses("");
        }
    }
    else if (method_name == "getProcess")
    {
        if ((args->size() > 0) && (args->get(0)->getType() == RpcValue::TypeInt))
        {
            return getProcess(args->get(0)->asInt());
        }
    }
    else if (method_name == "findProcess")
    {
        if ((args->size() > 0) && (args->get(0)->getType() == RpcValue::TypeString))
        {
            return findProcess(args->get(0)->asString());
        }
    }
    else if (method_name == "reboot")
    {
        System::Reboot();
    }
    else if (method_name == "execute")
    {
        // A String that represents 1 command
        if ((args->size() > 0) && (args->get(0)->getType() == RpcValue::TypeString))
        {
			uint64 pid = System::Execute(args->get(0)->asString());
			return new RpcValue((int)pid);
		}
    }
    else if (method_name == "getStatusOutput")
    {
        // A String that represents 1 command
        if ((args->size() > 0) && (args->get(0)->getType() == RpcValue::TypeString))
        {
            if ((args->size() > 1) && (args->get(1)->getType() == RpcValue::TypeString))
                return getStatusOutput(args->get(0)->asString(), args->get(1)->asString());
            else
                return getStatusOutput(args->get(0)->asString(), "");
        }
    }
    else if (method_name == "getCommandOutput")
    {
        // A String that represents 1 command
        if ((args->size() > 0) && (args->get(0)->getType() == RpcValue::TypeString))
        {
            if ((args->size() > 1) && (args->get(1)->getType() == RpcValue::TypeString))
                return getCommandOutput(args->get(0)->asString(), args->get(1)->asString());
            else
                return getCommandOutput(args->get(0)->asString(), "");
        }
    }
    else if (method_name == "runCommands")
    {
        // run each commands output in an array
        if ((args->size() > 0) && (args->get(0)->getType() == RpcValue::TypeArray))
        {
            RpcArrayValue *results = new RpcArrayValue();
            RpcArrayValue *commands = (RpcArrayValue*)args->get(0);
            for(uint32 i=0; i<commands->size(); ++i)
            {
                results->add(getCommandOutput(commands->get(i)->asString(), ""));
            }
            return (RpcValue*)results;
        }
    }
    else if (method_name == "version")
    {
        int major, minor, revision;
        SRL::GetVersion(major, minor, revision);
        return new RpcValue(String::Format("SRL Version: %d.%d.%d", major, minor, revision));
    }
    else if (method_name == "killProcess")
    {
        if ((args->size() > 0) && (args->get(0)->getType() == RpcValue::TypeInt))
        {
            int pid = args->get(0)->asInt();
            if (System::Kill(pid))
                return new RpcValue(true);
            return new RpcValue(false);
        }        
        return NULL;
    }
    else if (method_name == "pathExists")
    {
        if ((args->size() > 0) && (args->get(0)->getType() == RpcValue::TypeString))
        {
            return pathExists(args->get(0)->asString());
        }
    }
    else if (method_name == "writeToFile")
    {
        if (args->size() == 4)
        {
            return writeToFile(args->get(0)->asString(), args->get(1)->asString(), args->get(2)->asString(), args->get(3)->asBool());
        }
        else if (args->size() == 2)
        {
            return writeToFile(args->get(0)->asString(), args->get(1)->asString(), "");
        }
    }
    else if (method_name == "catFile")
    {
        if ((args->size() > 0) && (args->get(0)->getType() == RpcValue::TypeString))
        {
            String result;
            FS::CatTextFile(args->get(0)->asString(), &result);
            return new RpcValue(result);
        }
    }
    else if (method_name == "makeDir")
    {
        if (args->size() == 2)
        {
            System::UserInfo user_info;
            System::GetUserInfo(args->get(1)->asString(), user_info);
            String path = args->get(0)->asString();
            FS::MakeDir(path);
            FS::ChangeOwner(path, user_info.uid, user_info.gid);
            return new RpcValue(true);
        }
        else if (args->size() == 1)
        {
            FS::MakeDir(args->get(0)->asString());
            return new RpcValue(true);
        }
    }
    else if (method_name == "removeDir")
    {
        if ((args->size() > 0) && (args->get(0)->getType() == RpcValue::TypeString))
        {
            FS::Delete(args->get(0)->asString());
            return new RpcValue(true);
        }
    }
    else
    {
        return RpcService::execute(method_name, args);
    }
    
    return NULL;
}

RpcValue* RpcSystemService::getInfo()
{
    System::SystemInfo *sysinfo = System::SystemInfo::GetInstance();
    sysinfo->refresh();
    RpcStructValue *value = new RpcStructValue();
    value->set("hostname", sysinfo->name());
    value->set("os_name", sysinfo->os().name());
    value->set("os_kernel", sysinfo->os().kernel());
    value->set("os_version", sysinfo->os().version());
    value->set("cpu_count", sysinfo->cpuCount());
    value->set("cpu_model", sysinfo->cpu().modelName());
    value->set("cpu_speed", sysinfo->cpu().speed());
    value->set("cpu_physical", sysinfo->numberOfPhysicalCPUs());
    value->set("cpu_logical", sysinfo->numberOfLogicalCPUs());
    value->set("cpu_cores", sysinfo->numberOfCoresPerCPU());
    value->set("memory_total", BYTES_TO_MEGABYTES(sysinfo->memory().total()));
    DateTime dt;
    value->set("datetime", dt);
    value->set("uptime", sysinfo->strUptime());
    
    // TODO list mount points
    NetworkInterfaces* iface_list = NetworkInterface::GetAll();
    RpcArrayValue *interfaces = new RpcArrayValue();
    for (uint32 i=0; i<iface_list->size();++i)
    {
        NetworkInterface* iface = (*iface_list)[i];    
        RpcStructValue *ifacevalue = new RpcStructValue();
        ifacevalue->set("name", iface->name());
        ifacevalue->set("mac", iface->mac());
        ifacevalue->set("ip", iface->ip());
        String hd_type = "ethernet";
        if (iface->isLoopback())
            hd_type = "loopback";
        else if (iface->isWifi())
            hd_type = "wireless";        
        ifacevalue->set("type", hd_type);
        interfaces->add(ifacevalue);
    }
    value->set("network", interfaces);
    return value;
}

RpcValue* RpcSystemService::getUsage()
{
    System::SystemInfo *sysinfo = System::SystemInfo::GetInstance();
    sysinfo->refresh();
    RpcStructValue *value = new RpcStructValue();
    value->set("cpu_load", sysinfo->cpu().load());
    value->set("memory_total", BYTES_TO_MEGABYTES(sysinfo->memory().total()));
    value->set("memory_free", BYTES_TO_MEGABYTES(sysinfo->memory().free()));
    value->set("swap_total", BYTES_TO_MEGABYTES(sysinfo->swap().total()));
    value->set("swap_free", BYTES_TO_MEGABYTES(sysinfo->swap().free()));
    return value;    
}

RpcValue* RpcSystemService::getNetUsage()
{
    NetworkInterfaces* iface_list = NetworkInterface::GetAll();
    RpcArrayValue *interfaces = new RpcArrayValue();
    for (uint32 i=0; i<iface_list->size();++i)
    {
        NetworkInterface* iface = (*iface_list)[i];    
        RpcStructValue *ifacevalue = new RpcStructValue();
        ifacevalue->set("name", iface->name());
        ifacevalue->set("mac", iface->mac());
        ifacevalue->set("ip", iface->ip());
        RpcStructValue *ifrx = new RpcStructValue();
        
        ifacevalue->set("rx", ifrx);
        ifrx->set("bytes", (int)iface->rx().bytes());
        ifrx->set("packets", (int)iface->rx().packets());
        ifrx->set("multicast", (int)iface->rx().bytes());
        ifrx->set("errors", (int)iface->rx().packets());   
        ifrx->set("dropped", (int)iface->rx().dropped());        
        
        RpcStructValue *iftx = new RpcStructValue();
        ifacevalue->set("tx", iftx);
        iftx->set("bytes", (int)iface->tx().bytes());
        iftx->set("packets", (int)iface->tx().packets());
        iftx->set("multicast", (int)iface->tx().bytes());
        iftx->set("errors", (int)iface->tx().packets());   
        iftx->set("dropped", (int)iface->tx().dropped());
        
        interfaces->add(ifacevalue);
    }
    return interfaces;
}

RpcValue* RpcSystemService::configureNetwork(RpcArrayValue *args)
{
    // iface name, ip, netmask, broadcast
    return NULL;
}

RpcValue* RpcSystemService::getHistory(uint32 seconds)
{
    return NULL;
}

RpcValue* RpcSystemService::listProcesses(const String& user)
{
    System::ProcessList *proc_list = System::ProcessList ::GetInstance();
    proc_list->refresh();
    RpcArrayValue *value = new RpcArrayValue();
    System::ProcessList::Iterator iter = proc_list->begin();
    for (; iter!=proc_list->end(); ++iter)
    {
        System::ProcessList::Process *proc = iter.value();
        if (user.isEmpty() || (proc->owner() == user))
        {            
            RpcStructValue *procvalue = new RpcStructValue();
            procvalue->set("pid", proc->id());
            procvalue->set("name", proc->name());
            procvalue->set("parent_pid", proc->parentId());
            procvalue->set("owner", proc->owner());
            procvalue->set("cpu", proc->cpu().load()); 
            procvalue->set("memory_resident", proc->memory().residentSize()); 
            procvalue->set("memory_virtual", proc->memory().virtualSize());
            procvalue->set("threads", proc->threads()); 
            procvalue->set("handles", proc->handles()); 
            value->add(procvalue);  
        }
    }
    return value;
}

RpcValue* RpcSystemService::findProcess(const String& pname)
{
    System::ProcessList *proc_list = System::ProcessList ::GetInstance();
    proc_list->refresh();
    RpcArrayValue *value = new RpcArrayValue();
    System::ProcessList::Iterator iter = proc_list->begin();
    for (; iter!=proc_list->end(); ++iter)
    {
        System::ProcessList::Process *proc = iter.value();
        if (pname.isEmpty() || proc->name().isEqualTo(pname, true))
        {            
            RpcStructValue *procvalue = new RpcStructValue();
            procvalue->set("pid", proc->id());
            procvalue->set("name", proc->name());
            procvalue->set("parent_pid", proc->parentId());
            procvalue->set("owner", proc->owner());
            procvalue->set("cpu", proc->cpu().load()); 
            procvalue->set("memory_resident", proc->memory().residentSize()); 
            procvalue->set("memory_virtual", proc->memory().virtualSize());
            procvalue->set("threads", proc->threads()); 
            procvalue->set("handles", proc->handles()); 
            value->add(procvalue);  
        }
    }
    return value;    
}

RpcValue* RpcSystemService::getProcess(const int& pid)
{
    System::ProcessList *proc_list = System::ProcessList ::GetInstance();
    proc_list->refresh();
    System::ProcessList::Process *proc = proc_list->getProcess(pid);
    if (proc == NULL)
        throw SRL::Errors::IOException(String::Format("could not find process with pid: %d", pid));
    RpcStructValue *procvalue = new RpcStructValue();
    procvalue->set("pid", proc->id());
    procvalue->set("name", proc->name());
    procvalue->set("parent_pid", proc->parentId());
    procvalue->set("owner", proc->owner());
    procvalue->set("cpu", proc->cpu().load()); 
    procvalue->set("memory_resident", proc->memory().residentSize()); 
    procvalue->set("memory_virtual", proc->memory().virtualSize());
    procvalue->set("threads", proc->threads()); 
    procvalue->set("handles", proc->handles()); 
    return procvalue;    

}

RpcValue* RpcSystemService::getStatusOutput(const String& cmd, const String& user)
{
    int exit_status = 0;
    String buffer(1024);
    //printf("command: %s\n", cmd.text());
    if (user.isEmpty() || (user.size() < 2))
    {
        //printf("entering Run()\n");
        exit_status = System::RunEx(cmd, buffer);
    }
    else
    {
        //printf("entering RunAs(%s, %s)\n", cmd.text(), user.text());
        System::Sleep(1000);
        exit_status = System::RunAsEx(cmd, user, buffer);
    }

    RpcStructValue *procres = new RpcStructValue();
    procres->set("status", exit_status);
    procres->set("output", buffer);
    return procres;
}

RpcValue* RpcSystemService::getCommandOutput(const String& cmd, const String& user)
{
    String buffer(1024);
    //printf("command: %s\n", cmd.text());
    if (user.isEmpty() || (user.size() < 2))
    {
        //printf("entering Run()\n");
        System::Run(cmd, buffer);
    }
    else
    {
        //printf("entering RunAs(%s, %s)\n", cmd.text(), user.text());
        System::Sleep(1000);
        System::RunAs(cmd, user, buffer);
    }
    //printf("command output: '%s'\n", buffer.text());
    RpcValue *value = new RpcValue(buffer);
    //printf("rpc xml: %s\n", value->asXml().text());
    return value;
}

RpcValue* RpcSystemService::pathExists(const String& path)
{
    return new RpcValue(FS::Exists(path));
}

RpcValue* RpcSystemService::writeToFile(const String& filename, const String& text, const String& user, bool append)
{
    if (!append)
    {
        FS::WriteTextFile(filename, text, true);
    }
    else
    {
        FS::AppendToTextFile(filename, text);
    }

    if (!user.size() > 2)
    {
        System::UserInfo user_info;
        System::GetUserInfo(user, user_info);
        FS::ChangeOwner(filename, user_info.uid, user_info.gid);
    }
    return new RpcValue(true);
}


