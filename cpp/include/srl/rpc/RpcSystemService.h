#ifndef _RPCSYSTEMSERVICE_H_
#define _RPCSYSTEMSERVICE_H_

#include "srl/rpc/RpcProtocol.h"

namespace SRL
{
    namespace RPC
    {
        /**
        * RpcSystemService : public RpcService
        * 
        */
        class SRL_API RpcSystemService : public RpcService
        {
        public:
            /** default contstructor */
            RpcSystemService();
            /** default destructor */
            virtual ~RpcSystemService(){};

            /** 
            * called by the RPC protocol when the service name matches 
            * 
            * @param method_name    name of method to execute
            * @param args           Array of RpcValues that are arguments for the method
            * @return bool          returns an RpcValue if the method executed correctly else NULL
            * @overrides RpcServier::execute
            */
            RpcValue* execute(const String& method_name, RpcArrayValue *args);
            
            /** 
            * returns the current system information
            * - os, cpu, memory, disk drives, network interfaces
            */
            RpcValue* getInfo();
            /**
            * returns the current usage stats
            */
            RpcValue* getUsage();
            /**
            * returns the current network usage stats
            */
            RpcValue* getNetUsage();
            /**
            * configures a nework interface returning the results
            */
            RpcValue* configureNetwork(RpcArrayValue *args);            
            /**
            * returns the usage as an array dating back x number of seconds
            */
            RpcValue* getHistory(uint32 seconds);
            /**
            * returns the current process list for a user or if none all users 
            */
            RpcValue* listProcesses(const String& user);
            /**
            * returns a list of process that match the passed in name
            */
            RpcValue* findProcess(const String& pname);
            /**
            * returns a given process by its pid
            */
            RpcValue* getProcess(const int& pid);

            /**
            * runs a command and blocks until the cmd exits, then returns the output
            */
            RpcValue* getCommandOutput(const String& cmd, const String& user);
            
            /**
            * runs a command and blocks until the cmd exits, then returns the output and the exit code
            */
            RpcValue* getStatusOutput(const String& cmd, const String& user);
            
            /**
            * check if the path exists
            */
            RpcValue* pathExists(const String& path);
            /**
            * creates the specified path
            */
            RpcValue* mkdir(const String& path, const String& user);
            /**
            * writeToFile
            */
            RpcValue* writeToFile(const String& path, const String& text, const String& user, bool append=false);
        protected:
            
            
        };
        
    } /* RPC */
    
} /* SRL */


#endif /* _RPCSYSTEMSERVICE_H_ */
