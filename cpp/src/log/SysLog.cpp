
#include "srl/log/SysLog.h"
#include "srl/log/LogFile.h"
#include "srl/sys/System.h"

#if defined (linux) || (OSX)
#include <syslog.h>
#endif

using namespace SRL;
using namespace SRL::Log;

void SysLog::Create(const String& name, const Logger::Priority& loglevel)
{
#if defined (linux) || (OSX)
	// if we are using linux use syslog
	Logger::AddOutput(new SysLog(name, loglevel));
#elif defined(WIN32)
	// if we are running windows create a log in c:\name.log
	SRL::String windir = System::GetEnv("windir");
	char sysdrive = 'c';
	if (windir.length() > 0)
		sysdrive = windir[0];
	String log_filename = String::Format("%c:\\%s.log", sysdrive, name.text());
	LogFile::Create(log_filename, loglevel);

#endif
}

SysLog::SysLog(const String& name, const Logger::Priority& loglevel):
LoggerOutput(loglevel),
_syslog_name(name)
{

}
	
SysLog::~SysLog() throw()
{
	//::closelog();
}

#if defined (linux) || (OSX)
int GetSysLogLevel(const Logger::Priority& priority)
{
	switch (priority)
	{
		case (Logger::ALL):
			return LOG_EMERG;
		case (Logger::DEBUG):
			return LOG_DEBUG;
		case (Logger::INFO):
			return LOG_INFO;
		case (Logger::NOTICE):
			return LOG_NOTICE;
		case (Logger::WARN):
			return LOG_WARNING;
		case (Logger::ERR):
			return LOG_ERR;
		case (Logger::CRIT):
			return LOG_CRIT;
		case (Logger::ALERT):
			return LOG_ALERT;
		case (Logger::EMERG):
			return LOG_EMERG;
		case (Logger::FATAL):
			return LOG_EMERG;
	}
	return LOG_EMERG;
}

void SysLog::log(const Logger::Priority& priority,  const String& name, const String& message)
{
	_syslog_mutex.lock();
	::openlog(name.text(), LOG_PID, LOG_DAEMON);
	syslog(GetSysLogLevel(priority), String::Format("(%s) %s", 
	       Logger::GetPriorityName(priority).text(), message.text()).text());
	::closelog();
	_syslog_mutex.unlock();
}

#elif defined(WIN32)

void SysLog::log(const Logger::Priority& priority,  const String& name, const String& message)
{
	// do nothing
}

#endif
