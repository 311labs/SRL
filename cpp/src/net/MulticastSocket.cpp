
#include "srl/net/MulticastSocket.h"
#include "srl/sys/System.h"

using namespace SRL;
using namespace SRL::Net;

MulticastSocket::MulticastSocket() : DatagramSocket()
{

}

MulticastSocket::MulticastSocket(const int& port) : DatagramSocket(port)
{

}

MulticastSocket::MulticastSocket(const InetSocketAddress &addr) : DatagramSocket(addr)
{

}

MulticastSocket::~MulticastSocket()
{

}

uint8 MulticastSocket::getTimeToLive() const
{
	uint8 result;
	socklen_t rsize=sizeof(uint8);
	if (::getsockopt(_socket.getDescriptor().raw(), IPPROTO_IP, IP_MULTICAST_TTL, (char*)&result, &rsize)==-1)
	    throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
	return result;
}

void MulticastSocket::setTimeToLive(uint8 ttl)
{
	if (::setsockopt(_socket.getDescriptor().raw(), IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(uint8)) == -1)
	    throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );    
}

bool MulticastSocket::getLoopbackMode() const
{
	uint8 result;
	socklen_t rsize = sizeof(bool);
	if (::getsockopt(_socket.getDescriptor().raw(), IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&result, &rsize)==-1)
	    throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
	return result;
}

void MulticastSocket::setLoopbackMode(bool enable)
{
	if (::setsockopt(_socket.getDescriptor().raw(), IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&enable, sizeof(bool)) == -1)
	    throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
}

InetAddress MulticastSocket::getMulticastInterface()
{
    InetAddress addr;
    socklen_t rsize = sizeof(addr._addr);
	if (::getsockopt(_socket.getDescriptor().raw(), IPPROTO_IP, IP_MULTICAST_IF, (char*)&addr._addr, &rsize) == -1)
	    throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
	return addr;
}

void MulticastSocket::setMulticastInterface(const InetAddress& inf)
{
	if (::setsockopt(_socket.getDescriptor().raw(), IPPROTO_IP, IP_MULTICAST_IF, (char*)&inf._addr, sizeof(inf._addr)) == -1)
	    throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );

}

void MulticastSocket::joinGroup(const InetAddress& group)
{
    ip_mreq mgroup;
    mgroup.imr_multiaddr.s_addr = group._addr.s_addr;
    mgroup.imr_interface.s_addr = INADDR_ANY;
	if (::setsockopt(_socket.getDescriptor().raw(), IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mgroup, sizeof(mgroup)) == -1)
	    throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );    
}

void MulticastSocket::leaveGroup(const InetAddress& group)
{
    ip_mreq mgroup;
    mgroup.imr_multiaddr.s_addr = group._addr.s_addr;
    mgroup.imr_interface.s_addr = INADDR_ANY;
	if (::setsockopt(_socket.getDescriptor().raw(), IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mgroup, sizeof(mgroup)) == -1)
	    throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );    
}

