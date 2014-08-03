#include "srl/log/LoggerStream.h"

#include <iostream>

using namespace SRL;
using namespace SRL::Log;

LoggerStream::LoggerStream(Logger& logger, const Logger::Priority& priority) 
throw()
: _logger(&logger), _priority(priority), _buffer(NULL)
{}

LoggerStream::LoggerStream(const LoggerStream& other) throw()
{
	_copy(other);
}

LoggerStream::~LoggerStream()
{
	delete _buffer;
	_buffer = NULL;
}

LoggerStream& LoggerStream::operator=(const LoggerStream& rhs) throw()
{
	if (this != &rhs)
	{
		_copy(rhs);
	}

	return *this;
}

Logger& LoggerStream::logger() const throw()
{
	return *_logger;
}

Logger::Priority LoggerStream::priority() const throw()
{
	return _priority;
}

LoggerStream& LoggerStream::operator<<(const LoggerStream::Separator& s) 
throw()
{
	flush();
	return *this;
}

void LoggerStream::flush() throw()
{
	if (_buffer != NULL) 
	{
		_logger->log( _priority, _buffer->str() );
		delete _buffer;
		_buffer = NULL;
	}
}

void LoggerStream::_copy(const LoggerStream& other) throw()
{
	_logger = other._logger;
	_priority = other._priority;
	_buffer = NULL;
}

