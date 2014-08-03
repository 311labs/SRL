
#include "srl/net/NetworkInterface.h"
#include "srl/net/InetSocketAddress.h"
#include "srl/sys/System.h"

#include "srl/fs/TextFile.h"
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include "srl/net/wireless.h"

using namespace SRL;
using namespace SRL::Net;

void NetworkInterface::_update_interface(NetworkInterface* iface, bool is_new)
{
    ifreq ifr; 
    sockaddr_in *sin = (sockaddr_in *)&ifr.ifr_addr; 
    int sock_fd, i; 
    unsigned char *uc;
    uint32 flags = 0;
    
    bzero(&ifr, sizeof(ifr)); 

    if((sock_fd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        throw Errors::SocketException("creating socket failed!");
    } 
        
    // fill in our structure with the interface name
    strcpy(ifr.ifr_name, iface->name().text()); 
    sin->sin_family = AF_INET; 
        
    if (is_new)
    {
        // get the mac address
        if (::ioctl(sock_fd, SIOCGIFHWADDR, &ifr) == 0)
        {
            uc = (unsigned char *) &ifr.ifr_addr.sa_data; 
            iface->_device_type = ifr.ifr_hwaddr.sa_family;
            iface->_mac = String::Format("%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x", uc[0], uc[1], uc[2], uc[3], uc[4], uc[5]);
        }
        
        // get the device type
        switch (iface->_device_type)
        {
            case ARPHRD_LOOPBACK:
                iface->_type = LOOPBACK;
                break;
            case ARPHRD_ETHER:
            case ARPHRD_EETHER:
            case ARPHRD_IEEE802:
                iface->_type = WIRED;
                break;
            case ARPHRD_PPP:
                iface->_type = POINTTOPOINT;
                break;
            default:
                iface->_type = UNKNOWN;
                ::closesocket(sock_fd );
                return;
        }
    }   

    // get the routing information
    // get ip address
    if(::ioctl(sock_fd, SIOCGIFADDR, &ifr) == 0) 
    { 
        iface->_ip = ::inet_ntoa(sin->sin_addr);
    }
    
     // get broadcast 
    if (::ioctl(sock_fd, IFF_BROADCAST, &ifr) == 0)
    {
        sin = (sockaddr_in *)&ifr.ifr_addr; 
        iface->_broadcast = ::inet_ntoa(sin->sin_addr);
    }

     // get netmask 
     if (::ioctl(sock_fd, SIOCGIFNETMASK, &ifr) == 0)
     {
        sin = (sockaddr_in *)&ifr.ifr_addr; 
        iface->_netmask = ::inet_ntoa(sin->sin_addr);
     }
     
     // get the default gateway?
    // if (::ioctl(sock_fd, SIOCADDRT, &ifr) == 0)
    // {
    //  sin = (sockaddr_in *)&ifr.ifr_addr; 
    //  iface->_gateway = ::inet_ntoa(sin->sin_addr);
    // }
    
     // get flags
     if (::ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == 0)
     {   
        flags = ifr.ifr_flags;
         // check if is loopback
        iface->_is_up = (flags & IFF_UP);
     }
     
    // check if is wireless
    if (::ioctl(sock_fd, SIOCGIWNAME, &ifr) == 0)
    {
    
        iface->_type = WIRELESS;
        
        iwreq iw;
        char str[128];
        
        strcpy(iw.ifr_name, iface->_name.text());
        
        iw.u.essid.pointer = &str[0];
        iw.u.essid.length = 128;
        // get the essid
        if (::ioctl(sock_fd, SIOCGIWESSID, &iw) == 0)
        {
            iface->_essid = (const char*)iw.u.essid.pointer;
            //std::cout << "essid: " << iface->_essid.text() << std::endl;
        }
        
        // get the ap mac
        if (::ioctl(sock_fd, SIOCGIWAP, &ifr) == 0)
        {
            uc = (unsigned char *) &ifr.ifr_addr.sa_data; 
            iface->_ap_mac = String::Format("%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x", uc[0], uc[1], uc[2], uc[3], uc[4], uc[5]);
            //std::cout << "ap mac: " << iface->_ap_mac.text() << std::endl;

        }   
    }
     
    ::closesocket(sock_fd );
}

// bytes    packets errs drop fifo frame compressed multicast
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
    
    String data;
    FS::CatTextFile("/proc/net/dev", &data);
    // split the data string by the \n delimiter
    int count = data.count('\n');
    for (int i=2; i < count; ++i)
    {
        String line = data.section('\n', i);
        // parse the name
        String name = line.section(":", 0).trimBegin();
        String net_info = line.section(":", 1);
        NetworkInterface* iface = GetByName(name, false);
        if (iface == NULL)
        {
            iface = new NetworkInterface(name);
            __newifs->add(iface);
            _update_interface(iface, true);
            // only keep known devices
            if (iface->_type == UNKNOWN)
            {
                __newifs->remove(iface);
                delete iface;
                continue;
            }
        }
        else
        {
            _update_interface(iface, false);
        }

        new_list.add(iface);

        // parse the net iface info
        iface->_rx._bytes = String::UInt(net_info.section(' ', 0, false));
        iface->_rx._packets = String::UInt(net_info.section(' ', 1, false));
        iface->_rx._errors = String::UInt(net_info.section(' ', 2, false));
        iface->_rx._drops = String::UInt(net_info.section(' ', 3, false));
        iface->_rx._multicast = String::UInt(net_info.section(' ', 7, false));
        iface->_tx._bytes = String::UInt(net_info.section(' ', 8, false));
        iface->_tx._packets = String::UInt(net_info.section(' ', 9, false));
        iface->_tx._errors = String::UInt(net_info.section(' ', 10, false));
        iface->_tx._drops = String::UInt(net_info.section(' ', 11, false));
        iface->_tx._multicast = 0;
    }
    
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

