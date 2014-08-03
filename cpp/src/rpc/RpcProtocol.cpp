#include "srl/rpc/RpcProtocol.h"

using namespace SRL;
using namespace SRL::RPC;


RpcValue* RpcService::execute(const String& method_name, RpcArrayValue *args)
{
    if (hasKey(method_name))
    {
        return get(method_name)->execute(args);
    }
    // if this service does not have this method just return null
    return NULL;
}

/** Add a method to the service */
void RpcService::addMethod(RpcMethod& method)
{
    if (!hasKey(method.name()))
    {
        add(method.name(), &method);
        return;
    }
    throw SRL::Errors::IOException("failed to add method to service because a method with that name already exists");
}

/** removes a method from this service */
void RpcService::removeMethod(const String& method_name)
{
    if (!hasKey(method_name))
    {
        remove(method_name);
    }
}

/** tests if the rpc service contains the method */
bool RpcService::hasMethod(const String& method_name)
{
    return hasKey(method_name);
}

void RpcService::listMethods(RpcArrayValue &list)
{
    RpcMethodMap::Iterator iter = this->begin();
    for (; iter!=this->end(); ++iter)
    {
        list.add(String::Format("%s.%s", _name.text(), iter.key().text()));
    }
}


// RPC PROTOCOL

/** Add a method to the service */
void RpcProtocol::addService(RpcService& service)
{
    if (!_services.hasKey(service.name()))
    {
        _services.add(service.name(), &service);
        return;
    }
    throw SRL::Errors::IOException("failed to add service to protocol because a service with that name already exists");
}

/** removes a method from this service */
void RpcProtocol::removeService(const String& name)
{
    if (!_services.hasKey(name))
    {
        _services.remove(name);
    }
}

/** tests if the rpc service contains the method */
bool RpcProtocol::hasService(const String& name)
{
    return _services.hasKey(name);
}


RpcArrayValue* RpcProtocol::listMethods() const
{
    RpcArrayValue* method_list = new RpcArrayValue();
    RpcServiceMap::Iterator iter = _services.begin();
    for (; iter!=_services.end(); ++iter)
    {
        iter.value()->listMethods(*method_list);
    }
    return method_list;
}

RpcMethod* RpcProtocol::getMethod(const String& service_name, const String& method_name) const
{
    if (_services.hasKey(service_name))
    {
        RpcService* service = _services.get(service_name);
        if (service->hasMethod(method_name))
        {
            return service->get(method_name);
        }
    }
    
    return NULL;    
}

RpcValue* RpcProtocol::execute(const String& service_name, const String& method_name, RpcArrayValue* args)
{
    // RpcMethod* method = getMethod(service_name, method_name);
    // if (method != NULL)
    // {
    //     return method->execute(args);
    // }
    if (_services.hasKey(service_name))
    {
        RpcService* service = _services.get(service_name);
        return service->execute(method_name, args);
    }
    
    
    return NULL;
}

