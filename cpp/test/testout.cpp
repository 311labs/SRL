#include "srl/Console.h"
#include "srl/sys/Process.h"
#include "srl/sys/System.h"

using namespace SRL;


//int testPopen (const String& command)
//{
//    char line[128];
//    FILE *fp = ::popen(command.text(), "r");
//    do
//    {   
//        if (!::fgets( line, sizeof(line), fp))
//            break;
//        printf("%s\n", line);
//    } while (true);
//
//    ::pclose(fp);
//    return 0;
//}

void testProc(const String& command)
{
    System::Process proc(command);
    String line(1024);
    while (!proc.output().atEnd())
    {
        proc.output().readLine(line, false);
        Console::write(line);
        line.clear();
    }    
}

int main (int argc, char const *argv[])
{
    testProc("ping -c 5 -i 2 www.google.com");
    testProc("/XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXUsers/ians/Projects/srl/test.py -c 5 -d 2 hello world 'monkey man'");
    testProc("ping -c 5 -i 2 www.google.com");
    testProc("/Users/ians/Projects/srl/test.py -c 5 -d 2 hello world 'monkey man'");
    testProc("ping -c 5 -i 2 www.google.com");
    testProc("/Users/ians/Projects/srl/test.py -c 5 -d 2 hello world 'monkey man'");
    testProc("ping -c 5 -i 2 www.google.com");
    testProc("/Users/ians/Projects/srl/test.py -c 5 -d 2 hello world 'monkey man'");
    return 0;
}

