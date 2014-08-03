
#include "srl/log/LogFile.h"

using namespace SRL;
using namespace SRL::Log;

void LogFile::Create(const String& filename, const Logger::Priority& loglevel)
{
	try
	{
		File file(filename);
		// log system works by having a log for each day
		if ((FS::Exists(filename)) && (!file.createdDate().isToday()))
		{
			// need to move the log
			Date today;
			String mvlog = filename + "." + String::Val((int)today.day());
			FS::Move(filename, mvlog, true);
		}
	}
	catch(...)
	{

	}

	Logger::AddOutput(new LogFile(filename, loglevel));
}

LogFile::LogFile(const String& filename, const Logger::Priority& loglevel):
LoggerOutput(loglevel),
TextFile(filename, File::ASCII_READ_APPEND)
{
	
}
	
LogFile::~LogFile() throw()
{
	close();
}

void LogFile::log(const Logger::Priority& priority,  const String& name, const String& message)
{
	_sysTime.update();
	formatLine("%s>[%s] %s- %s", _sysTime.asString().text(), 
		Logger::GetPriorityName(priority).text(),  name.text(),  message.text());
	commit();
}
