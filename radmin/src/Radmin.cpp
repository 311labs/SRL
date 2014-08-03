#include "srl/app/ArgumentParser.h"
#include "srl/net/HttpServer.h"
#include "srl/Console.h"
#include "srl/log/Logger.h"
#include "srl/log/SysLog.h"
#include "srl/log/LogFile.h"
#include "srl/xml/XmlRpcProtocol.h"
#include "srl/rpc/RpcSystemService.h"
#include "srl/radmin/Radmin.h"
#include "srl/radmin/MonitoredProcess.h"
#include "srl/sys/ProcessList.h"

#ifdef WIN32
#include "srl/win/Registry.h"
#define REG_KEY_PATH "SOFTWARE\\SRL\\Radmin"
#define REG_KEY_DESKTOP "SYSTEM\\CurrentControlSet\\Services\\radmin"
#endif

#define START_FTP_PORT 30000
#define STOP_FTP_PORT 49000


using namespace SRL;
using namespace SRL::Net;
using namespace SRL::RPC;
using namespace SRL::Radmin;
using namespace SRL::App;

const char* METHOD_RESPONSE_HEADER = "<?xml version=\"1.0\"?>\n<methodResponse><params>";
const char* METHOD_RESPONSE_FOOTER = "</params></methodResponse>\n";

RadminDaemon::RadminDaemon(const String& config_path) :
System::Daemon("radmin"),
RpcService("radmin"),
_config(config_path),
_protocol(NULL),
_ftp_port(START_FTP_PORT-1)
{
	printf("config path: '%s'", config_path.text());
	if (!FS::Exists(config_path))
	{
		printf("CONFIG PATH DOES NOT EXIST %s", config_path.text());
	}

    // parse the config file here?
    add("monitorProcess", NULL);
    add("listMonitors", NULL);
    add("stopMonitor", NULL);
    add("stopAllMonitors", NULL);
    add("killAllProcs", NULL);
    add("sendFile", NULL);
    add("getFile", NULL);
}

RadminDaemon::~RadminDaemon()
{
    
}

int RadminDaemon::getNextFtpPort()
{
    _ftp_critsec.lock();
    ++_ftp_port;
    _ftp_critsec.unlock();
    return _ftp_port;
}

RpcValue* RadminDaemon::killAllProcs(const String& user)
{
    System::ProcessList *proc_list = System::ProcessList ::GetInstance();
    proc_list->refresh();
    RpcArrayValue *value = new RpcArrayValue();
    System::ProcessList::Iterator iter = proc_list->begin();
    for (; iter!=proc_list->end(); ++iter)
    {
        System::ProcessList::Process *proc = iter.value();
        if (proc->owner() == user)
        {            
            RpcStructValue *procvalue = new RpcStructValue();
            procvalue->set("pid", proc->id());
            procvalue->set("name", proc->name());
            procvalue->set("parent_pid", proc->parentId());
            procvalue->set("owner", proc->owner());
            value->add(procvalue);  
            proc->terminate(true);
        }
    }
    return value;
}

RpcValue* RadminDaemon::monitorProcess(RPC::RpcArrayValue *args)
{
    // this rpc call takes struct
    // command
    // uid
    // gid
    // array of keywords with actions
    if (args->size() > 0)
    {
        if (args->get(0)->getType() == RpcValue::TypeStruct)
        {
            RpcStructValue *proc_args = (RpcStructValue*)args->get(0);
            System::UserInfo user_info;
            user_info.uid = 0;
            String log_file = "";
                        
            if (proc_args->hasMember("user"))
            {
                
                System::GetUserInfo(proc_args->get("user")->asString(), user_info);
                log_file = user_info.home + "/";
            }
            _log.info("about to spawn new monitored process(%s)", proc_args->get("command")->asString().text());                
            MonitoredProcess *mproc = NULL;
            if (user_info.uid != 0)
                mproc = new MonitoredProcess(proc_args->get("command")->asString(), user_info);
            else
                mproc = new MonitoredProcess(proc_args->get("command")->asString());
            _log.info("spawning new monitored process(%s)", proc_args->get("command")->asString().text());
			
            // check if we should log to file file
            if (proc_args->hasMember("logfile"))
            {
                log_file += proc_args->get("logfile")->asString();
                mproc->setLogPath(log_file);
            }
                        
            // now for each keyword trigger add it to the monitor
            if (proc_args->hasMember("triggers"))
            {
                RpcArrayValue *triggers = (RpcArrayValue*)proc_args->get("triggers");
                for(uint32 i=0; i<triggers->size(); ++i)
                {
                    RpcStructValue *trigger = (RpcStructValue*)triggers->get(i);
                    mproc->addTrigger(trigger->get("name")->asString(), trigger->get("keyword")->asString(),
                        trigger->get("actions")->asInt());
                }
            }
            
			
			_log.info("starting process");
            mproc->addListener(this);
            mproc->start();
            _monitors.add(mproc);
            // wait for no more then 500 ms or until we have a pid
            for(uint32 d=0; d<10; ++d)
            {
                if (mproc->pid() > 0)
                {
                    _log.info("returning pid %u", mproc->pid());
                    return new RpcValue(mproc->pid());
                }
                System::Sleep(100);
            }
        }
    }
    _log.info("returning null");
    return NULL;
}
    
bool RadminDaemon::initial()
{
    IniParser::Section *section = _config.getSection("logging");
	String log_path = section->getString("path", "c:\\radmin.log");
	String log_level = section->getString("level", "ALL");
	log_level.toUpper();
	if (log_path == "SYSLOG")
	{
	    SRL::Log::Logger::SetOutput(new SRL::Log::SysLog("radmin"));
	}
	else
	{
		SRL::Log::Logger::SetOutput(new SRL::Log::LogFile(log_path));
	}

	if (log_level == "ALL")
	{
		Log::Logger::SetGlobalPriorityLevel(Log::Logger::ALL);
	}
	else if (log_level == "DEBUG")
	{
		Log::Logger::SetGlobalPriorityLevel(Log::Logger::DEBUG);
	}
	else if (log_level == "INFO")
	{
		Log::Logger::SetGlobalPriorityLevel(Log::Logger::INFO);
	}	
	else if (log_level == "WARN")
	{
		Log::Logger::SetGlobalPriorityLevel(Log::Logger::WARN);
	}

    _log.info("starting the radmin deamon");
    _gc.start();
    
    _protocol = new XML::XmlRpcProtocol();
    RpcSystemService *system_service = new RpcSystemService();
    _protocol->addService(*system_service);
    _protocol->addService(*this);

    _http_server = new Net::HttpServer(*_protocol);
    
    section = _config.getSection("xmlrpc");
    String host = section->getString("host", "localhost");
    int port = section->getInteger("port", 10000);
    _http_server->start(host, port);
    // need to sleep here to give time for the other thread to start up
    System::Sleep(1000);
    return true;
}


void RadminDaemon::createEventChannel()
{
    _log.info("creating event channel");
    IniParser::Section *section = _config.getSection("event channel");
    String host = section->getString("host", "");
    int port = section->getInteger("port", 80);
    
    int delay = 2000;
    do
    {   
        _event_channel.reset();
        try
        {
            System::Sleep(delay);
            // ramp the delay up between connection attempts
            if (delay < 40000)
                delay = delay + 200;
            _event_channel.connect(host, port);
        }
        catch (SRL::Errors::Exception &e)
        {
            _log.error(e);
            //s.close();
        }
        
        if (!_keep_running)
            return;
        
    } while (!_event_channel.isConnected());
    
    _log.info("connected to remote event listener");
    // now we need to send who we are to the remote host
    String xml = METHOD_RESPONSE_HEADER;
    ScopedPointer<RpcStructValue> rpcvalue(static_cast<RpcStructValue*>(_protocol->execute("system", "getInfo", NULL)));
    rpcvalue->set("rpc_protocol", "xmlrpc");
    rpcvalue->set("rpc_port", _config.getSection("xmlrpc")->getInteger("port", 80));
    xml += rpcvalue->asXml();
    xml += METHOD_RESPONSE_FOOTER;
    System::ScopedLock scoped_lock(_event_critsec);
    _event_channel.send(xml);
    _log.info("event channel created");    
}

bool RadminDaemon::run()
{
    // lets make sure the xml-rpc server is running
    if (!_http_server->isRunning())
    {
        _log.error("xml-rpc server is not runing, shutting down!");
        return false;
    }
    
    // let attempt to connect to our remote server
    if (!_event_channel.isConnected())
	{
        createEventChannel();
		if (!_keep_running)
			return false;
	}

    if (_event_channel.isRecvReady(60000))
    {
        // we should never have data coming the other way, assume its a disconnect
        _log.warn("remote event listener has gone away");
        _event_channel.close();
    }
    return true;
}

void RadminDaemon::final()
{
    _log.warn("radmin is shutting down");
    _http_server->stop();
    System::Sleep(500);
    delete _http_server;
}

void RadminDaemon::thread_exception(SRL::Errors::Exception &e)
{
	_log.error("Radmin thread received critical exception during run time");
	_log.error(e.message());
}

// RPC Callback
RPC::RpcValue* RadminDaemon::execute(const String& method_name, RPC::RpcArrayValue *args)
{
    if (method_name == "monitorProcess")
    {
        return monitorProcess(args);
    }
    else if (method_name == "killAllProcs")
    {
        if ((args->size() > 0) && (args->get(0)->getType() == RpcValue::TypeString))
        {
            return killAllProcs(args->get(0)->asString());
        }
        return NULL;
    }
    else if (method_name == "sendFile")
    {
        int ftp_port = 0;
        RPC::RpcValue* return_value = NULL;
        if (args->size() == 2)
        {
            ftp_port = getNextFtpPort();
            RadminFTP* ftp = new RadminFTP(ftp_port, args->get(0)->asString(),
                args->get(1)->asString(), false);
            _gc.add(ftp);
            return new RPC::RpcValue(ftp_port);
        }
        else if (args->size() == 1)
        {
            ftp_port = getNextFtpPort();
            RadminFTP* ftp = new RadminFTP(ftp_port, args->get(0)->asString(), false);
            _gc.add(ftp);
            return new RPC::RpcValue(ftp_port);
        }
        return return_value;
    }
    else if (method_name == "getFile")
    {
        int ftp_port = 0;
        RPC::RpcValue* return_value = NULL;
        if (args->size() == 2)
        {
            ftp_port = getNextFtpPort();
            RadminFTP* ftp = new RadminFTP(ftp_port, args->get(0)->asString(),
                args->get(1)->asString(), true);
            _gc.add(ftp);
            return new RPC::RpcValue(ftp_port);
        }
        else if (args->size() == 1)
        {
            ftp_port = getNextFtpPort();
            RadminFTP* ftp = new RadminFTP(ftp_port, args->get(0)->asString(), true);
            _gc.add(ftp);
            return new RPC::RpcValue(ftp_port);
        }
        return return_value;
    }

    
    return RpcService::execute(method_name, args);
}

// monitor call backs
void RadminDaemon::monitor_triggered(Monitor &monitor, Trigger &trigger)
{
    // ALL Communication to through the event channel must be synchronized 
    String xml = METHOD_RESPONSE_HEADER;
    ScopedPointer<RpcStructValue> rpcvalue(monitor.asRpcValue());
    rpcvalue->set("event", "trigger");
    rpcvalue->set("trigger", trigger.name());
    rpcvalue->set("trigger_type", trigger.type());
    rpcvalue->set("actions", trigger.actions());
    if (trigger.type() == "keyword")
    {
        KeywordTrigger *ktrig = static_cast<KeywordTrigger*>(&trigger);
        rpcvalue->set("keyword", ktrig->keyword());
        rpcvalue->set("match", ktrig->match());
        _log.info("monitor(%s) keyword triggered '%s'", monitor.name().text(), ktrig->keyword().text());    
    }
    else
    {
        _log.info("monitor(%s) triggered", monitor.name().text());
    }
    xml += rpcvalue->asXml();
    xml += METHOD_RESPONSE_FOOTER;
    System::ScopedLock scoped_lock(_event_critsec);
    _event_channel.send(xml); 
    
}

void RadminDaemon::monitor_error(Monitor &monitor, SRL::Errors::Exception &e)
{
    String errmsg = String::Format("monitor(%s) error '%s'", monitor.name().text(), e.message().text());
    _log.error(errmsg);
    
    RpcStructValue fault;
    fault.set("faultCode", -1);
    fault.set("faultString", errmsg);    
    
    String xml = METHOD_RESPONSE_HEADER;
    xml += fault.asXml();
    xml += METHOD_RESPONSE_FOOTER;
    _monitors.remove(&monitor);
    _gc.add(&monitor);

    System::ScopedLock scoped_lock(_event_critsec);
    _event_channel.send(xml);
    

}

void RadminDaemon::monitor_stopped(Monitor &monitor)
{
    _log.info("monitor(%s) stopped", monitor.name().text());
    String xml = METHOD_RESPONSE_HEADER;
    ScopedPointer<RpcStructValue> rpcvalue(monitor.asRpcValue());
    rpcvalue->set("event", "stopped");
    xml += rpcvalue->asXml();
    xml += METHOD_RESPONSE_FOOTER;
    _event_channel.send(xml);
    _monitors.remove(&monitor);
    _gc.add(&monitor);
}

void RadminDaemon::stop_event()
{
    _log.info("stop event received");
    stop();
}

void RadminDaemon::reload_event()
{
    _log.info("reload event received");
    stop();
}

GarbageCollector::GarbageCollector() : System::Thread()
{
    setSleep(60000);
}

GarbageCollector::~GarbageCollector()
{
    _garbage.clear(true);
    _ftp_garbage.clear(true);
}

void GarbageCollector::add(Monitor *monitor)
{
    _critsec.lock();
    _garbage.add(monitor);
    _critsec.unlock();
}

void GarbageCollector::add(RadminFTP *ftp)
{
    _critsec.lock();
    _ftp_garbage.add(ftp);
    _critsec.unlock();
}

bool GarbageCollector::run()
{
    if (_garbage.size() > 0)
    {
        _critsec.lock();
        printf("deleting %d monitors\n", _garbage.size());
        _garbage.clear(true);
        _critsec.unlock();
    }
    
    if (_ftp_garbage.size() > 0)
    {
        _critsec.lock();
        printf("checking %d ftp clients\n", _ftp_garbage.size());

        Util::Vector<RadminFTP*>::Iterator iter = _ftp_garbage.begin();
        for (; iter != _ftp_garbage.end(); iter++)
        {
            if (iter->isDone())
            {
                iter = _ftp_garbage.erase(iter);
            }
        }
        _critsec.unlock();
    }
    
    return true;
}






int main (int argc, char const* argv[])
{
    int major, minor, revision;
    SRL::GetVersion(major, minor, revision);
    String srl_version = String::Format("%d.%d.%d", major, minor, revision);
    
    App::ArgumentParser argparser("SRL Radmin Service", 
        "A remote admin service that provides a simple XML-RPC interface to a remote host", 
        srl_version);
    argparser.add("verbose", "-v, --verbose", "display verbose output");
	argparser.add("test", "--test", "test radmin on the console");
    argparser.add("install", "-i, --install", "installs radmin as a system service");
	argparser.add("uninstall", "-u, --uninstall", "uninstalls radmin as a system service");
	argparser.add("start", "--start", "starts the radmin system service");
    argparser.add("stop", "--stop", "stops the radmin system service");
	argparser.add("config", "-c, --config", "location of the configuration file", "radmin.conf");
    argparser.add("help", "--help", "display this output");
    argparser.parse(argc, argv);
     
    if (argparser.get("verbose")->wasParsed())
    {
        Log::Logger::SetGlobalPriorityLevel(Log::Logger::ALL);
    }
	
	SRL::String config_path = argparser.get("config")->getValue().text();
	Console::formatLine("\tconfig path: %s", config_path.text());
#ifdef WIN32

	if (!argparser.get("config")->wasParsed())
	{
		config_path.reserve(128);
		if (Windows::Registry::HasKey(HKEY_LOCAL_MACHINE, REG_KEY_PATH))
			Windows::Registry::GetValue(HKEY_LOCAL_MACHINE, REG_KEY_PATH, "config", config_path);
	}
	Console::formatLine("\tsetting config path: %s", config_path.text());
#endif

    RadminDaemon daemon(config_path);
    if (argparser.get("install")->wasParsed())
    {
		Console::writeLine("installing radmin as a system service...");
		try
		{
#ifdef WIN32
		Windows::Registry::CreateKey(HKEY_LOCAL_MACHINE, REG_KEY_PATH);
		Windows::Registry::SetValue(HKEY_LOCAL_MACHINE, REG_KEY_PATH, "config", config_path);
#endif
		Console::writeLine("\tinstall service");
        daemon.install(argv[0]);
#ifdef WIN32        
		Console::writeLine("\tenabling desktop interaction");
        int32 reg_type = 0;
        Windows::Registry::GetValue(HKEY_LOCAL_MACHINE, REG_KEY_DESKTOP, "Type", reg_type);
        reg_type = reg_type | 256;
        Windows::Registry::SetValue(HKEY_LOCAL_MACHINE, REG_KEY_DESKTOP, "Type", &reg_type);
#endif
		Console::writeLine("radmin has been succesfully installed as a system service");
		}
		catch (SRL::Errors::Exception &e)
		{
			Console::err::writeLine(e.message());
			Console::err::dumpStack();
		}
		
    }
    else if (argparser.get("uninstall")->wasParsed())
    {
		try
		{
			Console::write("removing radmin from system services: ");
			daemon.uninstall();
			Console::writeLine("OK");
		}
		catch (SRL::Errors::Exception &e)
		{
			Console::err::writeLine(e.message());
		}
	}
	else if (argparser.get("start")->wasParsed())
    {
		Console::write("starting radmin: ");
		daemon.start();
		Console::writeLine("OK");
	}
    else if (argparser.get("stop")->wasParsed())
    {
		Console::write("stopping radmin: ");
		daemon.stop();
		Console::writeLine("OK");
	}
    else if (argparser.get("test")->wasParsed())
    {
        daemon.runForever();
		Console::writeLine("radmin exited run forever");
	}
	else
	{
#ifdef WIN32
		daemon.start();
#else
        daemon.runForever();
		Console::writeLine("radmin exited run forever");
#endif
	}

    return 0;
}
