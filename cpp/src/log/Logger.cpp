
#include "srl/log/Logger.h"
#include <cstdio>
#include <cstdarg>
#include "srl/Console.h"

using namespace SRL;
using namespace SRL::Log;

Util::Vector<LoggerOutput*> Logger::__Outputs;
Logger::LoggerMap Logger::__Map;
Logger::Priority Logger::__GlobalPriorityLevel = Logger::FATAL;
System::Mutex __mutex;

Logger& Logger::GetInstance(const String& loggerName)
{
    System::ScopedLock scopedlock(__mutex);
	Logger* ptr = NULL;
    
	if (!__Map.hasKey(loggerName))
	{
		ptr = new Logger(loggerName);
		__Map[loggerName] = ptr;
	}
	else
	{
		ptr = __Map[loggerName];
	}

	// check to make sure we have a output
	if (Logger::__Outputs.size() == 0)
		LoggerOutput::Create(Logger::INFO);
	return *ptr;
}

void Logger::AddOutput(LoggerOutput *output)
{
    System::ScopedLock scopedlock(__mutex);
	__Outputs.add(output);
}

void Logger::SetOutput(LoggerOutput *output)
{
    System::ScopedLock scopedlock(__mutex);
    __Outputs.clear(true);
    __Outputs.add(output);
}

void Logger::SetGlobalPriorityLevel(const Logger::Priority& priorityLevel) 
throw()
{
	__GlobalPriorityLevel = priorityLevel;
}

String Logger::name() const throw()
{
	return _name;
}

Logger::Priority Logger::priorityLevel() const throw()
{
	return _priorityLevel;
}

void Logger::setPriorityLevel(const Logger::Priority& priorityLevel) throw()
{
	if (_priorityLevel != priorityLevel)
	{
		_priorityMutex.lock();

		_priorityLevel = priorityLevel;

		_priorityMutex.unlock();
	}
}

//LoggerStream Logger::stream(const Logger::Priority& priority) const throw()
//{
//	return LoggerStream(*this, priority);
//}

void Logger::log(const Logger::Priority& priority, 
			const String& message) const throw()
{
	if ( (priority >= _priorityLevel) || 
		(priority >= __GlobalPriorityLevel) )
	{
		_log(priority, message);
	}
}

void Logger::log(const Logger::Priority& priority, 
				 const char* stringFormat, ...) const throw()
{
	if ( (priority >= _priorityLevel) || 
		(priority >= __GlobalPriorityLevel) )
	{
		va_list va;
		va_start(va, stringFormat);
		_logva(priority, stringFormat, va);
		va_end(va);
	}
}

void Logger::debug(const String& message) const throw()
{
	log(DEBUG, message);
}

void Logger::debug(const char* stringFormat, ...) const throw()
{
	if ( (DEBUG >= _priorityLevel) || 
		(DEBUG >= __GlobalPriorityLevel) )
	{
		va_list va;
		va_start(va, stringFormat);
		_logva(DEBUG, stringFormat, va);
		va_end(va);
	}
}

//LoggerStream Logger::debugStream() const throw()
//{
//	return stream(DEBUG);
//}

void Logger::info(const String& message) const throw()
{
	log(INFO, message);
}

void Logger::info(const char* stringFormat, ...) const throw()
{
	if ( (INFO >= _priorityLevel) || 
		(INFO >= __GlobalPriorityLevel) )
	{
		va_list va;
		va_start(va, stringFormat);
		_logva(INFO, stringFormat, va);
		va_end(va);
	}
}

//LoggerStream Logger::infoStream() const throw()
//{
//	return stream(INFO);
//}

void Logger::notice(const String& message) const throw()
{
	log(NOTICE, message);
}

void Logger::notice(const char* stringFormat, ...) const throw()
{
	if ( (NOTICE >= _priorityLevel) || 
		(NOTICE >= __GlobalPriorityLevel) )
	{
		va_list va;
		va_start(va, stringFormat);
		_logva(NOTICE, stringFormat, va);
		va_end(va);
	}
}

//LoggerStream Logger::noticeStream() const throw()
//{
//	return stream(NOTICE);
//}

void Logger::warn(const String& message) const throw()
{
	log(WARN, message);
}

void Logger::warn(const char* stringFormat, ...) const throw()
{
	if ( (WARN >= _priorityLevel) || 
		(WARN >= __GlobalPriorityLevel) )
	{
		va_list va;
		va_start(va, stringFormat);
		_logva(WARN, stringFormat, va);
		va_end(va);
	}
}

//LoggerStream Logger::warnStream() const throw()
//{
//	return stream(WARN);
//}

void Logger::error(const Errors::Exception& err) const throw()
{
	log(ERR, err.message());
}

void Logger::error(const String& message) const throw()
{
	log(ERR, message);
}

void Logger::error(const char* stringFormat, ...) const throw()
{
	if ( (ERR >= _priorityLevel) || 
		(ERR >= __GlobalPriorityLevel) )
	{
		va_list va;
		va_start(va, stringFormat);
		_logva(ERR, stringFormat, va);
		va_end(va);
	}
}

//LoggerStream Logger::errorStream() const throw()
//{
//	return stream(ERR);
//}

void Logger::crit(const String& message) const throw()
{
	log(CRIT, message);
}

void Logger::crit(const char* stringFormat, ...) const throw()
{
	if ( (CRIT >= _priorityLevel) || 
		(CRIT >= __GlobalPriorityLevel) )
	{
		va_list va;
		va_start(va, stringFormat);
		_logva(CRIT, stringFormat, va);
		va_end(va);
	}
}

//LoggerStream Logger::critStream() const throw()
//{
//	return stream(CRIT);
//}

void Logger::alert(const String& message) const throw()
{
	log(ALERT, message);
}

void Logger::alert(const char* stringFormat, ...) const throw()
{
	if ( (ALERT >= _priorityLevel) || 
		(ALERT >= __GlobalPriorityLevel) )
	{
		va_list va;
		va_start(va, stringFormat);
		_logva(ALERT, stringFormat, va);
		va_end(va);
	}
}

//LoggerStream Logger::alertStream() const throw()
//{
//	return stream(ALERT);
//}

void Logger::emerg(const String& message) const throw()
{
	log(EMERG, message);
}

void Logger::emerg(const char* stringFormat, ...) const throw()
{
	if ( (EMERG >= _priorityLevel) || 
		(EMERG >= __GlobalPriorityLevel) )
	{
		va_list va;
		va_start(va, stringFormat);
		_logva(EMERG, stringFormat, va);
		va_end(va);
	}
}

//LoggerStream Logger::emergStream() const throw()
//{
//	return stream(EMERG);
//}

void Logger::fatal(const String& message) const throw()
{
	log(FATAL, message);
}

void Logger::fatal(const char* stringFormat, ...) const throw()
{
	if ( (FATAL >= _priorityLevel) || 
		(FATAL >= __GlobalPriorityLevel) )
	{
		va_list va;
		va_start(va, stringFormat);
		_logva(FATAL, stringFormat, va);
		va_end(va);
	}
}

//LoggerStream Logger::fatalStream() const throw()
//{
//	return stream(FATAL);
//}

Logger::Logger(const String& loggerName) throw()
: _name(loggerName)
, _priorityLevel(INFO)
, _priorityMutex(), _logMutex()
{}

Logger::~Logger() throw()
{
	if (__Map.hasKey(this->name()))
	{
		__Map.remove(this->name());
	}
}

void Logger::_logva(const Logger::Priority& priority, 
			const char* format, va_list va) const throw()
{
	// Fixed size char array...not good
	char tmp[512];
	vsprintf(tmp, format, va);

	_log( priority, String(tmp) );
}

String Logger::GetPriorityName(const Logger::Priority& priority) throw()
{
	switch(priority)
	{
	case DEBUG:
		return "debug";
	case INFO:
		return "info";
	case NOTICE:
		return "notice";
	case WARN:
		return "warning";
	case ERR:
		return "error";
	case CRIT:
		return "critical";
	case ALERT:
		return "alert";
	case EMERG:
		return "emergency";
	case FATAL:
		return "fatal";
	default:
		return "";
	}
}

void Logger::_log(const Priority& priority, const String& message) const throw()
{
    for (uint32 i=0; i < __Outputs.size();++i)
    {
        __Outputs[i]->log(priority, _name, message);
    }
}




void LoggerOutput::Create(const Logger::Priority& loglevel)
{
	Logger::AddOutput(new LoggerOutput(loglevel));
}

LoggerOutput::LoggerOutput( const Logger::Priority& loglevel):
_loglevel(loglevel)
{
	
}
	
void LoggerOutput::log(const Logger::Priority& priority,  const String& name, const String& message)
{ 
	_mutex.lock();
	_dateTime.update();
	Console::write("<");
	Console::write(_dateTime.asString());
	Console::write(">");

	Console::write("[");
    Console::write(name);
    Console::write(":");    
    Console::write(Logger::GetPriorityName(priority));
	Console::write("] ");
    
    Console::writeLine(message);
	_mutex.unlock();
}
