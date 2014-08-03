
#ifndef _LOGGERSTREAM_H_
#define _LOGGERSTREAM_H_

#include "srl/log/Logger.h"

namespace SRL
{
    namespace Log
    {
	/**
	* @author Shane Frasier
	* $Author: ians $
	* $Revision: 258 $
	* $Date: 2006-04-10 18:45:20 -0400 (Mon, 10 Apr 2006) $
	* @short Stream class for Logger
	*/
	class SRL_API LoggerStream
	{
	public:
		/** An enumerated type for separators.  Currently only contains the 
			ENDL separator, which separates two log messages. */
		typedef enum
		{
			ENDL
		} Separator;
	
	public:
		/** Constructor */
		LoggerStream(Logger& logger, const Logger::Priority& priorityloglevel=Logger::INFO) throw();
		/** Copy constructor */
		LoggerStream(const LoggerStream& other) throw();
		/** Destructor */
		virtual ~LoggerStream();
		/** Assignment operator */
		LoggerStream& operator=(const LoggerStream& rhs) throw();
		/** Returns the Logger to which this LoggerStream corresponds */
		Logger& logger() const throw();
		/** Returns the priority of this LoggerStream */
		Logger::Priority priority() const throw();
		/** The << operator for writing to the stream */
		template<class T> 
			LoggerStream& operator<<(const T& t) throw(std::bad_alloc)
		{
			if (_buffer == NULL)
			{
				_buffer = new std::ostringstream;
			}
			
			(*_buffer) << t;
			
			return *this;
		}
		/** A specialized << operator for Separators */
		LoggerStream& operator<<(const Separator& s) throw();
		/** Used to flush the LoggerStream buffer */
		void flush() throw();
	
	private:
		/** The Logger to which this LoggerStream corresponds */
		Logger* _logger;
		/** The Logger::Priority of this LoggerStream */
		Logger::Priority _priority;
		/** Our std::ostringstream buffer */
		std::ostringstream* _buffer;
		/** Copies another LoggerStream object to this one */
		void _copy(const LoggerStream& other) throw();
		/** Default constructor declared private so it cannot be called */
		LoggerStream() throw();
	};
    }
}

#endif /* _LOGGERSTREAM_H_ */
