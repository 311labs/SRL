
#include "SystemTest.h"

#include "srl/sys/System.h"
#include "srl/sys/Process.h"

using namespace SRL;
using namespace SRL::Test;

#ifdef WIN32
#define ENV_USER "USERNAME"
#define PING_CMD "ping -n 100 -w 1000 localhost"
#else
#define ENV_USER "USER"
#define PING_CMD "ping -c 100 -i 2 localhost"
#endif

#include "srl/Console.h"

bool SystemTest::infoTest()
{
    // Test Host Name
    String hostname = System::GetHostName();
    // basically all we can do is test that it returned something
	TEST_ASSERT(!hostname.isEmpty())
	
	// Test User Name
    String username = System::GetUserName();
    // basically all we can do is test that it returned something
	TEST_ASSERT(!hostname.isEmpty())
	
    // Test Env Var
    String env_user = System::GetEnv(ENV_USER);
    // basically all we can do is test that it returned something
	TEST_ASSERT(!env_user.isEmpty())   
	
	// Test if user matches env user
	TEST_ASSERT(env_user == username)
	
	return true;
}

bool SystemTest::runkillTest()
{
    //.test current pid (make sure its not 0)
    TEST_ASSERT(System::GetPid() != 0)
    
    // test spawning a process
    String output;
    System::Run("hostname", output);
    output.trimEnd();
    TEST_ASSERT(output == System::GetHostName())
    // test executing a process
    uint64 pid = System::Execute(PING_CMD);
    // test if the process is running
    TEST_ASSERT(System::IsRunning(pid))
    
    // now see if we can kill the process
    TEST_ASSERT(System::Kill(pid))
    TEST_ASSERT(!System::IsRunning(pid))
    
    return true;
}

bool SystemTest::runProcs()
{
#ifndef WIN32
    String buffer(1024);
    buffer.clear();
    System::Run("/usr/bin/whoami", buffer);
    buffer.trim();
    TEST_ASSERT(System::GetUserName() == buffer);
    
    System::Process proc("bin/testout");
    
    String line(1024);
    proc.output().readLine(line, false);
    // this is stdout
    // this is stderr
    TEST_ASSERT(line.beginsWith("this is stdout"));

    line.clear();
    proc.output().readLine(line, false);
    TEST_ASSERT(line.beginsWith("this is stderr"));
        
#endif    
    return true;
}

