
#include "srl/net/NetworkInterface.h"
#include "srl/net/InetSocketAddress.h"
#include "srl/sys/System.h"

#include <Iphlpapi.h>

#include <algorithm>
#include <iostream>

using namespace SRL;
using namespace SRL::Net;

// first create a socket and extract basic interface
void get_socket_info(INTERFACE_INFO *socket_info, int &total)
{
	// WSA SOCKET STUFF

	SOCKET sd = ::WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
	DWORD ret_bytes = 0;
	
	if (::WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, socket_info, sizeof(socket_info), 
		&ret_bytes, 0, 0) == SOCKET_ERROR)
	{
		std::cerr << ::WSAGetLastError() << std::endl;
		throw Net::Errors::SocketException("WSAIoctl SIO_GET_INTERFACE_LIST failed!");
	}

	// number of interfaces found
	total = ret_bytes / sizeof(INTERFACE_INFO);

	::closesocket(sd);
}

// get MIB interface info 
void get_mib_iptable(MIB_IPADDRTABLE *ip_table)
{
	DWORD size = sizeof(MIB_IPADDRTABLE) * 19;
	DWORD res = ::GetIpAddrTable(ip_table, &size, FALSE);
	if (res != NO_ERROR)
		throw Net::Errors::SocketException("GetIpAddrTable failed!");

}

int32 get_mib_index(const String &ip, MIB_IPADDRTABLE *ip_table)
{
	in_addr *addr;
	String local_ip;

	for (int i=0; i < ip_table->dwNumEntries; ++i)
	{
		addr = (in_addr*)&ip_table->table[i].dwAddr;
		local_ip = ::inet_ntoa(*addr);
		if (local_ip == ip)
			return ip_table->table[i].dwIndex;
	}
	return -1;
}

bool NetworkInterface::__update()
{
	// TODO throw exceptions

	if (__interfaces == NULL)
	{
		// TODO call WSACleanup
		SRL::Net::Initialize();	
		__interfaces = new NetworkInterfaces(6);
		__newifs = new NetworkInterfaces(6);
		__deadifs = new NetworkInterfaces(6);
	}
	
    // only allow updates every 1 second at most
    float64 cur_time = System::GetSystemTimer();
    float64 delta_time = (cur_time - __last_update);
    
    if (delta_time < 1.0f)
        return false;
        
    __last_update = cur_time;	
	
	NetworkInterfaces new_list;
	// delete all the dead ones
	// now delete the dead ones
	_delete_dead();
	__newifs->clear();

	// WSA SOCKET STUFF
	INTERFACE_INFO socket_info[20];
	MIB_IPADDRTABLE ip_table[20];

	SOCKET sd = ::WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
	DWORD ret_bytes = 0;
	
	if (::WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, socket_info, sizeof(socket_info), 
		&ret_bytes, 0, 0) == SOCKET_ERROR)
	{
		std::cerr << ::WSAGetLastError() << std::endl;
		throw Net::Errors::SocketException("WSAIoctl SIO_GET_INTERFACE_LIST failed!");
	}

	// number of interfaces found
	int total = ret_bytes / sizeof(INTERFACE_INFO);
	::closesocket(sd);

	get_mib_iptable(ip_table);

	// IP ADDRESS TABLE
	MIB_IFROW if_row;
	int wired_count = 0;
	for (int i = 0; i < total; ++i)
	{
		// get the ipaddress from the socket generated list
		sockaddr_in *addr;
		addr = (sockaddr_in *) & (socket_info[i].iiAddress);
		String ip = inet_ntoa(addr->sin_addr);
		// get the MIB index
		int32 index = get_mib_index(ip, ip_table);
		if (index == -1)
		{
			std::cerr << "failed to get record for : " << ip << std::endl;
			continue;
		}

		if_row.dwIndex = (DWORD)index;
		if (::GetIfEntry(&if_row) != NO_ERROR)
		{
			std::cerr << "failed to get entry for index: " << index << std::endl;
			continue;
		}
		
		// get the name from the socket generated list
		String name;
		ADAPTOR_TYPE a_type;
		uint32 flags = socket_info[i].iiFlags;
        	if (flags & IFF_LOOPBACK)
		{		
			a_type = LOOPBACK;
			name = "lo";
		}		
		else if (flags & IFF_POINTTOPOINT)
		{		
			a_type = POINTTOPOINT;
			name = "ppp";
		}		
		else
		{
			a_type = WIRED;
			name = String::Format("eth%d", wired_count);
			++wired_count;
		}

		// check if we have an interface by this name already
		NetworkInterface* iface = GetByName(name, false);
		
		// no interface already, then lets create one
		if (iface == NULL)
		{
			iface = new NetworkInterface(name);
			__newifs->add(iface);
			iface->_mac = String::Format("%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x", 
			if_row.bPhysAddr[0],if_row.bPhysAddr[1],if_row.bPhysAddr[2],
			if_row.bPhysAddr[3],if_row.bPhysAddr[4],if_row.bPhysAddr[5]);
			
			iface->_mtu = if_row.dwMtu;
			iface->_type = a_type;
		}
		new_list.add(iface);

		iface->_speed = if_row.dwSpeed;

		iface->_rx._bytes = if_row.dwInOctets;
		iface->_rx._packets = if_row.dwInUcastPkts;
		iface->_rx._errors = if_row.dwInErrors;
		iface->_rx._drops = if_row.dwInDiscards;
		iface->_rx._multicast = if_row.dwInNUcastPkts;

		iface->_tx._bytes = if_row.dwOutOctets;
		iface->_tx._packets = if_row.dwOutUcastPkts;
		iface->_tx._errors = if_row.dwOutErrors;
		iface->_tx._drops = if_row.dwOutDiscards;
		iface->_tx._multicast = if_row.dwOutNUcastPkts;

        	//bool supports_mcast = (flags & IFF_MULTICAST);
        	//bool supports_bcast = (flags & IFF_BROADCAST);
		iface->_ip = ip;
        addr = (sockaddr_in *) & (socket_info[i].iiBroadcastAddress);
        iface->_broadcast = inet_ntoa(addr->sin_addr);
		addr = (sockaddr_in *) & (socket_info[i].iiNetmask);
		iface->_netmask = inet_ntoa(addr->sin_addr);

		switch (if_row.dwOperStatus)
		{
		case (MIB_IF_OPER_STATUS_NON_OPERATIONAL):
			iface->_is_up = false;
			iface->_has_link = false;
			break;
		case (MIB_IF_OPER_STATUS_UNREACHABLE):
			iface->_is_up = false;
			iface->_has_link = false;
			break;
		case (MIB_IF_OPER_STATUS_DISCONNECTED):
			iface->_is_up = false;
			iface->_has_link = false;
			break;
		case (MIB_IF_OPER_STATUS_CONNECTING):
			iface->_is_up = false;
			iface->_has_link = true;
			break;
		case (MIB_IF_OPER_STATUS_CONNECTED):
		case (MIB_IF_OPER_STATUS_OPERATIONAL):
			iface->_is_up = true;
			iface->_has_link = true;
			break;
		}

    }
	_remove_dead(&new_list);
	return true;
}


void NetworkInterface::setIP(const String& ip)
{
    //AddIPAddress
}

void NetworkInterface::setNetmask(const String& ip)
{
    //AddIPAddress
}

void NetworkInterface::setBroadcast(const String& ip)
{
    //AddIPAddress
}


/*
bool NetworkInterface::__update()
{
	if (__interfaces == NULL)
	{
		__interfaces = new NetworkInterfaces;
		__newifs = new NetworkInterfaces;
		__deadifs = new NetworkInterfaces;
	}
	NetworkInterfaces new_list;
	
	// delete all the dead ones
	_delete_dead();
	__newifs->clear();

	PIP_ADAPTER_INFO adapter_list;
	PIP_ADAPTER_INFO adapter = NULL;
	DWORD res = 0;

	// unsure how big to make the adapter_info list
	// so lets make it the size of one then get a buffer_overflow and resize
	ULONG req_size = sizeof(IP_ADAPTER_INFO);
	adapter_list = (PIP_ADAPTER_INFO) ::malloc(req_size);
	if (::GetAdaptersInfo(adapter_list, &req_size))
	{
		::free(adapter_list);
		adapter_list = (PIP_ADAPTER_INFO) ::malloc(req_size);
	}

	// now lets fill our adapter_list with data
	if (::GetAdaptersInfo( adapter_list, &req_size) == NO_ERROR)
	{
		int count = 0;
		adapter = adapter_list;
		while (adapter)
		{
			String name = String::Format("eth%d", count);
			++count;
			// check if we have an interface by this name already
			NetworkInterface* iface = GetByName(name, false);
			// no interface already, then lets create one
			if (iface == NULL)
			{
				iface = new NetworkInterface(name);
				__newifs->push_back(iface);
				iface->_mac = String::Format("%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x", 
				adapter->Address[0],adapter->Address[1],adapter->Address[2],
				adapter->Address[3],adapter->Address[4],adapter->Address[5]);
			}
			new_list.push_back(iface);

			iface->_index = count;
			iface->_ip = adapter->IpAddressList.IpAddress.String;
			iface->_netmask = adapter->IpAddressList.IpMask.String;
			iface->_gateway = adapter->GatewayList.IpAddress.String;
			
			adapter = adapter->Next;
		}
	}
	_remove_dead(&new_list);
	return true;
}
*/


