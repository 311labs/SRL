#include "srl/log/Logger.h"
#include "srl/log/LogFile.h"
#include "srl/log/SysLog.h"

using namespace SRL;
using namespace SRL::Log;

int main(int argc, char *argv[])
{
    // Set the Global log level up to INFO
    Logger::SetGlobalPriorityLevel(Logger::INFO);
    // get in instance to a logger called LogExample
    Logger &log = Logger::GetInstance("LogExample");
    // the default output is to stdout/stderr
    log.info("%s is the name of this program and stdout logger", argv[0]);

    // lets tell our logs to go to file instead
    String log_file = String::Format("%s.log", argv[0]);
    Logger::SetOutput(new LogFile("logexample.log"));
    log.info("%s is the name of this program and file logger", argv[0]);
    
    // lets tell our logs to go to the system log instead
    Logger::SetOutput(new SysLog("LogExample"));
    log.info("%s is the name of this program and system logger", argv[0]);
}
