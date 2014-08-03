
#include "srl/net/InetAddress.h"

#define INVALID_ADDRESS 0xFFFFFFFF

using namespace SRL;
using namespace SRL::Net;

Util::Vector<InetAddress*>* InetAddress::GetAllAByName(const String &name)
{
    Util::Vector<InetAddress*> *list = new Util::Vector<InetAddress*>();
    Net::Initialize();
    hostent *he = ::gethostbyname( name.text() );
    if (he == NULL)
    {
        throw Errors::InvalidAddressException("invalid address: " + name);  
    }
    
    in_addr **addr_list = (in_addr **)he->h_addr_list;
    for(int i = 0; addr_list[i] != NULL; ++i) 
    {
        InetAddress *inetaddr = new InetAddress(*addr_list[i]);
        list->add(inetaddr);
    }
    return list;
}

InetAddress::InetAddress()
{
    Net::Initialize();
    _addr.s_addr = INVALID_ADDRESS;
}

InetAddress::InetAddress(in_addr &addr)
{
    Net::Initialize();
    _addr = addr; 
}

InetAddress::InetAddress(const String& host)
{
    Net::Initialize();
    hostent *hp = ::gethostbyname( host.text() );
    if (hp != NULL)
    {
        ::memcpy( &_addr.s_addr, hp->h_addr, hp->h_length );
    }
    else
    {
		_addr.s_addr = INVALID_ADDRESS;
		throw Errors::InvalidAddressException("invalid address: " + host);        
    }
}

InetAddress::InetAddress(const InetAddress& addr)
{
    this->_addr = addr._addr;
    this->_ip = addr._ip;
    this->_hostname = addr._hostname;
}

InetAddress::~InetAddress()
{
    
}

const SRL::byte* InetAddress::getAddress() const
{
   return (SRL::byte*)&_addr.s_addr;
}

const String& InetAddress::getHostAddress() const
{
    if (_ip.length() == 0)
    {
        _ip = ::inet_ntoa(_addr);
    }
    return _ip;
}

const String& InetAddress::getHostName() const
{
    if (_hostname.length() == 0)
    {
        hostent *he = ::gethostbyaddr((char*)&_addr, sizeof(_addr), AF_INET);
        if (he != NULL)
            _hostname = he->h_name;
    }
    return _hostname;
}

const String& InetAddress::getCanonicalHostName() const
{
    if (_hostname.length() == 0)
    {
        hostent *he = ::gethostbyaddr((char*)&_addr, sizeof(_addr), AF_INET);
        if (he != NULL)
            _hostname = he->h_name;
    }
    return _hostname;
}

bool InetAddress::isValid() const
{
    return (_addr.s_addr != INVALID_ADDRESS);
}
