
#include "srl/net/NetworkInterface.h"
#include "srl/net/InetSocketAddress.h"
#include "srl/fs/TextFile.h"
#include "srl/sys/System.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <errno.h>


using namespace SRL;
using namespace SRL::Net;

void NetworkInterface::_update_interface(NetworkInterface* iface, bool is_new)
{
    // ifreq ifr; 
    // sockaddr_in *sin = (sockaddr_in *)&ifr.ifr_addr; 
    // int sock_fd, i; 
    // unsigned char *uc;
    // uint32 flags = 0;
    // 
    // bzero(&ifr, sizeof(ifr)); 
    // 
    // if((sock_fd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    // { 
    //  throw Errors::SocketException("creating socket failed!");
    // } 
    //  
    // // fill in our structure with the interface name
    // strcpy(ifr.ifr_name, iface->name().text()); 
    // sin->sin_family = AF_INET; 
    // 
    // // get the routing information
    //      // get ip address
    // if(::ioctl(sock_fd, SIOCGIFADDR, &ifr) == 0) 
    // { 
    //  iface->_ip = ::inet_ntoa(sin->sin_addr);
    // }
    // 
    //  // get broadcast 
    // if (::ioctl(sock_fd, IFF_BROADCAST, &ifr) == 0)
    // {
    //  sin = (sockaddr_in *)&ifr.ifr_addr; 
    //  iface->_broadcast = ::inet_ntoa(sin->sin_addr);
    // }
    // 
    //  // get netmask 
    //  if (::ioctl(sock_fd, SIOCGIFNETMASK, &ifr) == 0)
    //  {
    //      sin = (sockaddr_in *)&ifr.ifr_addr; 
    //      iface->_netmask = ::inet_ntoa(sin->sin_addr);
    //  }
    //  
    //  // get the default gateway?
    // if (::ioctl(sock_fd, SIOCADDRT, &ifr) == 0)
    // {
    //  sin = (sockaddr_in *)&ifr.ifr_addr; 
    //  iface->_gateway = ::inet_ntoa(sin->sin_addr);
    // }
    // 
    // ::closesocket(sock_fd);
}

bool NetworkInterface::__update()
{
	if (__interfaces == NULL)
	{
		__interfaces = new NetworkInterfaces;
		__newifs = new NetworkInterfaces;
		__deadifs = new NetworkInterfaces;
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
	
	struct ifaddrs *myaddrs, *ifa;
    struct sockaddr_in *s4;
    struct sockaddr_in6 *s6;
    struct if_data *idata;
    int status;
    /* buf must be big enough for an IPv6 address (e.g. 3ffe:2fa0:1010:ca22:020a:95ff:fe8a:1cf8) */
    char buf[64];
	
	if (getifaddrs(&myaddrs) != 0)
	{
	    throw Errors::SocketException("getifaddrs failed!");
	}
	
    unsigned char *uc;
    char buffer[124];
    bool is_new = false;
	for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next)
	{
        is_new = false;
        //printf("'%s'\t", ifa->ifa_name);
        if ((ifa->ifa_flags & IFF_UP) == 1)
        {
            //printf("\tup ");
        } 
        else
        {
            //printf("\tdown ");
            continue;
        }
        
        NetworkInterface* iface = GetByName(ifa->ifa_name, false);
		if (iface == NULL)
		{		    
		    for (uint32 i = 0; i < __newifs->size(); ++i)
    		{	
    			iface = (*__newifs)[i];
                //printf("'%s', ",iface->name().text());
    			if (iface->name() == ifa->ifa_name)
    			{
                    is_new = true;
                    break;
    		    }
    		}
    		
    		
    		
    		
    		if (is_new == false)
    		{
                //printf("new interface\n");
			    iface = new NetworkInterface(ifa->ifa_name);
                is_new = true;
                __newifs->add(iface);
                //printf(" size=%d ", __newifs->size());
            }
			//_update_interface(iface, true);
			// only keep known devices
			if (iface->_type == UNKNOWN)
			{
                //printf("removed\t");
				__newifs->remove(iface);
				delete iface;
				continue;
			}
		}
		else
		{
		    //printf("\n%s: %s-%s\n", iface->name().text(), iface->ip().text(), iface->mac().text());
		}

	    if (ifa->ifa_addr == NULL) continue;
        
        if (ifa->ifa_addr->sa_family == AF_LINK)
        {
            new_list.add(iface);

            if (is_new)
            {
		        uc = (unsigned char *) &ifa->ifa_addr->sa_data; 
		        iface->_device_type = ifa->ifa_addr->sa_family;
		        iface->_mac = String::Format("%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x", uc[0], uc[1], uc[2], uc[3], uc[4], uc[5]);
                //printf(iface->_mac.text());
            
                if ((ifa->ifa_flags & IFF_LOOPBACK) != 0)
                {
                    iface->_type = LOOPBACK;
                    //printf("\tloopback");
                }
            
            }
            
            idata = (struct if_data*)ifa->ifa_data;
            iface->_mtu = idata->ifi_mtu;
            iface->_metric = idata->ifi_metric;
            iface->_speed = idata->ifi_baudrate;
            iface->_collisions = idata->ifi_collisions;
    		
            iface->_rx._bytes = idata->ifi_ibytes;
    		iface->_rx._packets = idata->ifi_ipackets;
    		iface->_rx._errors = idata->ifi_ierrors;
    		iface->_rx._drops = idata->ifi_iqdrops;
            iface->_rx._multicast = idata->ifi_imcasts;

            iface->_tx._bytes = idata->ifi_obytes;
    		iface->_tx._packets = idata->ifi_opackets;
    		iface->_tx._errors = idata->ifi_oerrors;
    		iface->_tx._drops = 0;
            iface->_tx._multicast = idata->ifi_omcasts;            
        }
        else if (ifa->ifa_addr->sa_family == AF_INET)
        {
            s4 = (struct sockaddr_in *)(ifa->ifa_addr);
            inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), buffer, 124);
            iface->_ip = buffer;

            s4 = (struct sockaddr_in *)(ifa->ifa_netmask);
            inet_ntop(ifa->ifa_netmask->sa_family, (void *)&(s4->sin_addr), buffer, 124);
            iface->_netmask = buffer;

            s4 = (struct sockaddr_in *)(ifa->ifa_dstaddr);
            inet_ntop(ifa->ifa_dstaddr->sa_family, (void *)&(s4->sin_addr), buffer, 124);
            iface->_broadcast = buffer;



	    }
        else if (ifa->ifa_addr->sa_family == AF_INET6)
        {
            //s4 = (struct sockaddr_in *)(ifa->ifa_addr);
            //inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), buffer, 124);
            //iface->_ip = buffer;
            //printf("inet6\t");

	    }
	    else
	    {
            //printf("family: %d\t", ifa->ifa_addr->sa_family);
	    }
        //printf("\n\n");
	}
	
	freeifaddrs(myaddrs);
	
	_remove_dead(&new_list);
    return true;
}


int set_ip_using(const String& ifname, int c, const String& ip)
{
    int sock_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0) 
    { 
        throw SRL::Net::Errors::SocketException("creating socket failed!");
    }    
    
    ifreq ifr;
    sockaddr_in sin;

    strncpy(ifr.ifr_name, ifname.text(), ifname.length());
    memset(&sin, 0, sizeof(struct sockaddr));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(ip.text());
    if (sin.sin_addr.s_addr == -1)
    {
        throw SRL::Net::Errors::SocketException("invalid ip address!");
    }
    
    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
    int res = ioctl(sock_fd, c, &ifr);
    ::closesocket(sock_fd );
    if (res < 0)
        return -1;
    return 0;
}

void NetworkInterface::setIP(const String& ip)
{
    if (set_ip_using(this->_name, SIOCSIFADDR, ip) == -1)
    {
        throw SRL::Net::Errors::SocketException("failed to set ip address!");
    }
}

void NetworkInterface::setNetmask(const String& ip)
{
    if (set_ip_using(this->_name, SIOCSIFNETMASK, ip) == -1)
    {
        throw SRL::Net::Errors::SocketException("failed to set netmask address!");
    }
}

void NetworkInterface::setBroadcast(const String& ip)
{
    if (set_ip_using(this->_name, SIOCSIFBRDADDR, ip) == -1)
    {
        throw SRL::Net::Errors::SocketException("failed to set broadcast address!");
    }
}

