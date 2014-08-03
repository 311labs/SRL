
#include "srl/net/NetworkInterface.h"
#include "srl/net/InetSocketAddress.h"
#include "srl/fs/TextFile.h"

using namespace SRL;
using namespace SRL::Net;

NetworkInterfaces *NetworkInterface::__interfaces = NULL;
NetworkInterfaces *NetworkInterface::__newifs = NULL;
NetworkInterfaces *NetworkInterface::__deadifs = NULL;
float64 NetworkInterface::__last_update = 0.0;

NetworkInterface::NetworkInterface(String name) : 
_name(name),
_has_link(false),
_is_up(false),
_type(WIRED)
{
	
}

NetworkInterface::~NetworkInterface()
{

}

bool NetworkInterface::update()
{
	return __update();
}

NetworkInterfaces* NetworkInterface::GetAll(bool do_update)
{
	if (do_update)
		__update();
	return __interfaces;
}

NetworkInterface* NetworkInterface::GetByName(const String& name, bool do_update)
{
	if ((__interfaces != NULL)&&(!do_update))
	{
		for (uint32 i = 0; i < __interfaces->size(); ++i)
		{	
			NetworkInterface* iface = (*__interfaces)[i];
			if (iface->name() == name)
				return iface;
		}
	}
	if (do_update)
	{
		__update();
		// and repeat same code from above (blah copy paste)
		for (uint32 i = 0; i < __interfaces->size(); ++i)
		{	
			NetworkInterface* iface = (*__interfaces)[i];
			if (iface->name() == name)
				return iface;
		}
	}
	return NULL;
}


void NetworkInterface::_remove_dead(NetworkInterfaces* new_list)
{
	// go thru the current list of interfaces and look for dead interfaces
	NetworkInterfaces::Iterator iter = __interfaces->begin();
	while (iter != __interfaces->end())
	{
		NetworkInterface *iface = iter.value();
		if (!new_list->containsValue(iface))
		{
			// interface has gone down
			__deadifs->add(iface);
			iter = __interfaces->erase(iter);
		}
		else
			++iter;
	}
	
	// now add all the new interfaces to the list
	for (iter = __newifs->begin(); iter != __newifs->end(); ++iter)
	{
		__interfaces->add(iter.value());
	}
}

void NetworkInterface::_delete_dead()
{
	for (NetworkInterfaces::Iterator iter = __deadifs->begin(); iter != __deadifs->end(); ++iter)
	{
		delete iter.value();
	}
	__deadifs->clear();
}
