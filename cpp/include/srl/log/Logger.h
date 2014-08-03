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
#ifndef __SRL_Logger__
#define __SRL_Logger__

#include "srl/String.h"
#include "srl/DateTime.h"
#include "srl/sys/Mutex.h"
#include "srl/util/Dictionary.h"
#include "srl/util/Vector.h"
#include "srl/Exceptions.h"

// UNDEFINE DEBUG
#ifdef DEBUG
#undef DEBUG
#define DEBUG DEBUG
#endif

namespace SRL
{
	/**
	* @author Ian Starnes
	* @short Logging Framework
	*/
	namespace Log
	{
	
	class LoggerOutput;
	
	
	/**
	* @author Shane Frasier
	* $Author: ians $
	* $Revision: 258 $
	* $Date: 2006-04-10 18:45:20 -0400 (Mon, 10 Apr 2006) $
	* @short Generic logging class.
	*/
	class SRL_API Logger
	{
	public:
		/** An enumerated type for message priorities */
		typedef enum
		{
			ALL=0,
			DEBUG=1,
			INFO,
			NOTICE,
			WARN,
			ERR,
			CRIT,
			ALERT,
			EMERG,
			FATAL
		} Priority;
	
	public:
		/** Add a Logger Output*/
		static void AddOutput(LoggerOutput *output);
		/** Set Logger Output (clears out all old outputs) */
		static void SetOutput(LoggerOutput *output);
		/** Returns an instance to the Logger with name loggerName */
		static Logger& GetInstance(const String& loggerName);
		/** Sets the global priority level of all Loggers */
		static void SetGlobalPriorityLevel(const Priority& priorityLevel) 
			throw();
		/** Sets the global priority level of all Loggers */
		static void SetGlobalPriorityLevel(const int& priorityLevel) 
			throw() {SetGlobalPriorityLevel(static_cast<Priority>(priorityLevel));};
		/** Returns the string associated with the specified priority */
		static String GetPriorityName(const Logger::Priority& priority) throw();
	private:
		/** The pool of outputs */
		static Util::Vector<LoggerOutput*> __Outputs;
		/** We typedef this type for convenience */
		typedef Util::Dictionary<String, Logger*> LoggerMap;
		/** Associative array that allows us to look up a Logger by its name */
		static LoggerMap __Map;
	
	public:
		/** Returns the name of the Logger */
		String name() const throw();
		/** Returns the priority level of this Logger */
		Priority priorityLevel() const throw();
		/** Sets the priority level of this Logger */
		void setPriorityLevel(const Priority& priorityLevel) throw();
		/** Returns a LoggerStream corresponding to the specified priority */
		//LoggerStream stream(const Priority& priority) const throw();
		/** Used to log a message with the specified priority */
		void log(const Priority& priority, const String& message) const throw();
		/** Used to log a message with the specified priority 
		See Kernighan and Ritchie p. 244 and p. 246 for the printf/scanf
		format control characters */
		void log(const Priority& priority, 
			const char* stringFormat, ...) const throw();
		/** Used to log a debug message */
		void debug(const String& message) const throw();
		/** Used to log a debug message */
		void debug(const char* stringFormat, ...) const throw();
		/** Returns a stream for logging debug messages */
		//LoggerStream debugStream() const throw();
		/** Used to log an info message */
		void info(const String& message) const throw();
		/** Used to log an info message */
		void info(const char* stringFormat, ...) const throw();
		/** Returns a stream for logging info messages */
		//LoggerStream infoStream() const throw();
		/** Used to log a notice message */
		void notice(const String& message) const throw();
		/** Used to log a notice message */
		void notice(const char* stringFormat, ...) const throw();
		/** Returns a stream for logging notice messages */
		//LoggerStream noticeStream() const throw();
		/** Used to log a warning message */
		void warn(const String& message) const throw();
		/** Used to log a warning message */
		void warn(const char* stringFormat, ...) const throw();
		/** Returns a stream for logging warning messages */
		//LoggerStream warnStream() const throw();
		/** Used to log an error message */
		void error(const String& message) const throw();
		/** Used to log an error message */
		void error(const char* stringFormat, ...) const throw();
		/** logging an SRL exception */
		void error(const Errors::Exception &err) const throw();
		/** Returns a stream for logging error messages */
		//LoggerStream errorStream() const throw();
		/** Used to log a critical message */
		void crit(const String& message) const throw();
		/** Used to log a critical message */
		void crit(const char* stringFormat, ...) const throw();
		/** Returns a stream for logging critical messages */
		//LoggerStream critStream() const throw();
		/** Used to log an alert message */
		void alert(const String& message) const throw();
		/** Used to log an alert message */
		void alert(const char* stringFormat, ...) const throw();
		/** Returns a stream for logging alert messages */
		//LoggerStream alertStream() const throw();
		/** Used to log an emergency message */
		void emerg(const String& message) const throw();
		/** Used to log an emergency message */
		void emerg(const char* stringFormat, ...) const throw();
		/** Returns a stream for logging emergency messages */
		//LoggerStream emergStream() const throw();
		/** Used to log a fatal message */
		void fatal(const String& message) const throw();
		/** Used to log a fatal message */
		void fatal(const char* stringFormat, ...) const throw();
		/** Returns a stream for logging fatal messages */
		//LoggerStream fatalStream() const throw();
	
	protected:
		/** Constructor */	
		Logger(const String& loggerName) throw();
		/** Destructor */
		virtual ~Logger() throw();
		/** Used to handle printf-style argument lists */
		void _logva(const Priority& priority, 
			const char* format, va_list va) const throw();
	
	private:
		/** The name associated with this logger */
		String _name;
		/** The priority level of this Logger */
		Logger::Priority _priorityLevel;
		/** A mutex for thread safety when setting the priority */
		System::Mutex _priorityMutex;
		/** A mutex for thread safety when writing logs */
		mutable System::Mutex _logMutex;
		/** Default constructor declared private so it cannot be called */
		Logger() throw();
		/** Copy constructor declared private so it cannot be called */
		Logger(const Logger& other) throw();
		/** Assignment operator declared private so it cannot be called */
		Logger& operator=(const Logger& rhs) throw();
	
		/** The global priority level of all Loggers */
		static Logger::Priority __GlobalPriorityLevel;
		/** Logs a meaasge without checking the priority.  This is useful if 
		we have already checked the priority. */
		void _log(const Priority& priority, const String& message) const 
			throw();
	};
	
	/**
	* @author Ian Starnes
	* $Author: ians $
	* $Revision: 258 $
	* $Date: 2006-04-10 18:45:20 -0400 (Mon, 10 Apr 2006) $
	* @short Generic Logger Output to COUT
	*/
	class SRL_API LoggerOutput
	{
	public:
	    virtual ~LoggerOutput(){}
		/** public contructor */
		LoggerOutput(const Logger::Priority& logLevel=Logger::INFO);
		/** Add output to type to Logger */
		static void Create(const Logger::Priority& loglevel);
		/** main log method */
		virtual void log(const Logger::Priority& priority,  const String& name, const String& message);
		/** comparison operator needed by all types */
		friend bool operator==(const LoggerOutput& s1,const LoggerOutput& s2){ return (&s1 == &s2); };
		/** set log level */
		void setLogLevel(const Logger::Priority& logLevel);
		/** return this outputs current log level */
		const Logger::Priority getLogLevel(){ return _loglevel;};
		
	protected:
		/** keep the logging thread safe */
		System::Mutex _mutex;
	
		/** Date Time Stamp */
		DateTime _dateTime;
	
		/** the level to log to this output */
		Logger::Priority _loglevel;
	};
	
	} // namespace IO

} // namespace SRL

#endif // __SRL_Logger__
