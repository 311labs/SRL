#include "srl/net/DatagramSocket.h"
#include "srl/sys/System.h"

using namespace SRL;
using namespace SRL::Net;
using namespace SRL::Net::Errors;

DatagramSocket::DatagramSocket() : _socket(Socket::UDP)
{

}

DatagramSocket::DatagramSocket(const int& port) : _socket(Socket::UDP)
{
	InetSocketAddress addr(port);
	_socket.bind(addr);
}

DatagramSocket::DatagramSocket(const InetSocketAddress &addr) : _socket(Socket::UDP)
{
	_socket.bind(addr);
}

DatagramSocket::~DatagramSocket()
{
	
}

int DatagramSocket::sendTo(const InetSocketAddress &to_addr, const SRL::String &data, uint32 flags)
{
	return sendTo(to_addr, data.bytes(), data.length(), flags);
}

int DatagramSocket::sendTo(const InetSocketAddress &to_addr, const SRL::byte *data, uint32 len, uint32 flags)
{
    return _socket.sendTo(to_addr, data, len, flags);
}

int DatagramSocket::recvFrom(InetSocketAddress &from_addr, SRL::byte *buffer, uint32 len, uint32 flags)
{
    return _socket.recvFrom(from_addr, buffer, len, flags);
}

