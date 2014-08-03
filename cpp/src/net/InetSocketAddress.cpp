#include "srl/net/InetSocketAddress.h"

using namespace SRL;
using namespace SRL::Net;

InetSocketAddress::InetSocketAddress() : _port(-1)
{
    
}

InetSocketAddress::InetSocketAddress(const int& port)
{
    in_addr a;a.s_addr = INADDR_ANY;
    InetAddress addr(a);
    _initialize(addr, port);    
}

InetSocketAddress::InetSocketAddress(const InetAddress& addr, const int& port)
{
    _initialize(addr, port);
}

InetSocketAddress::InetSocketAddress(const String& hostname, const int port)
{
    InetAddress addr(hostname);
    _initialize(addr, port);
}

void InetSocketAddress::_initialize(const InetAddress& addr, const int& port)
{
    _port = port;
    // make a copy of the inetaddress
    _inet_addr = addr;
    ::memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = _inet_addr._addr.s_addr;
    _addr.sin_port = htons(_port);
    
}

InetSocketAddress::~InetSocketAddress()
{
    
}

String InetSocketAddress::asString() const
{
    return String::Format("%s:%d", _inet_addr.getHostAddress().text(), _port);
}
