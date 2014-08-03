#ifndef _RPCPROTOCOL_H_
#define _RPCPROTOCOL_H_

#include "srl/util/Dictionary.h"
#include "srl/rpc/RpcValue.h"

namespace SRL
{
    namespace RPC
    {
        /**
        * RpcMethod
        * Base class for RPC methods
        */
        class SRL_API RpcMethod
        {
        public:
            /** default contstructor */
            RpcMethod(const String& name, const String& help) : _name(name), _help(help) {};
            /** default destructor */
            virtual ~RpcMethod(){};
            
            /** returns the name of the method */
            const String& name() const { return _name; }
            /** returns the help string for this method */
            const String& help() const { return _help; }
            /**
            * Executes the RPC Method returning the result
            * 
            * @param args   RPC parameters to pass to method
            * @returns the results to send back to the caller
            */
            virtual RpcValue* execute(RpcValue* args)=0;
            
        protected:
            String _name, _help;
            
        };
        
        
        typedef Util::Dictionary<String, RpcMethod*> RpcMethodMap;
        /**
        * RpcService
        * Base class for an RPC Service, normally an RPC service is nothing more
        * then a collection of related rpc methods
        */
        class SRL_API RpcService : public RpcMethodMap
        {
        public:
            /** default contstructor */
            RpcService(const String& name) : _name(name){};
            /** default destructor */
            virtual ~RpcService(){};
            
            /** returns the name of this service */
            const String& name() const { return _name; }

            /** 
            * called by the RPC protocol when the service name matches 
            * 
            * @param method_name    name of method to execute
            * @param args           Array of RpcValues that are arguments for the method
            * @return bool          returns an RpcValue if the method executed correctly else NULL
            */
            virtual RpcValue* execute(const String& method_name, RpcArrayValue *args);
            
            /** Add a method to the service */
            void addMethod(RpcMethod& method);
            /** removes a method from this service */
            void removeMethod(const String& method_name);
            /** tests if the rpc service contains the method */
            bool hasMethod(const String& method_name);
            /** list method adds all of the methods that this service handles to a list */
            void listMethods(RpcArrayValue &list);
            
        protected:
            // name of service
            String _name;
            
        };
        
        
        typedef Util::Dictionary<String, RpcService*> RpcServiceMap;
        /**
        * RpcProtocol
        * Base class for RPC Protocols
        * 
        * This class automatically creates a "system" service that handles:
        *   list_methods(), methodHelp(name), methodSignature(name), methodInfo(name)
        */
        class SRL_API RpcProtocol
        {
        public:
            /** default contstructor */
            RpcProtocol(const String& name) : _name(name){};
            /** default destructor */
            virtual ~RpcProtocol(){};
            
            /** Add a method to the service */
            void addService(RpcService& method);
            /** removes a method from this service */
            void removeService(const String& name);
            /** tests if the rpc service contains the method */
            bool hasService(const String& name);
            
            /** 
            * returns a list of service.method_names 
            * the returned object should be freed by the caller with a delete
            */
            RpcArrayValue* listMethods() const;
        
            /** returns a method based on service and method name if none exists returns NULL */
            RpcMethod* getMethod(const String& service_name, const String& method_name) const;
        
            /**
            * Will automagically call the correct service and method
            */
            RpcValue* execute(const String& service_name, const String& method_name, RpcArrayValue* args);
        
        protected:
            String _name;
            RpcServiceMap _services;
            
        };
        
        
        
    } /* RPC */
    
} /* SRL */


#endif /* _RPCPROTOCOL_H_ */
