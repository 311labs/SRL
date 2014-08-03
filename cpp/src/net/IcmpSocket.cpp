
#include "srl/net/IcmpSocket.h"
#include <errno.h>

using namespace SRL;
using namespace SRL::Net;
using namespace SRL::Net::ICMP;
using namespace SRL::Net::Errors;

IcmpSocket::IcmpSocket(uint16 id) :
_socket(Socket::ICMP),
_echo_request(new RawMessage(ICMP::Message::ECHO_REQUEST, 0, 0, 32, 0)),
_echo_reply(new RawMessage(1024)),
_listener(NULL),
_uid(id)
{
	_socket.setReceiveBufferSize(60 * 1024);
}

IcmpSocket::~IcmpSocket()
{
	delete _echo_request;
	delete _echo_reply;
}

int IcmpSocket::sendTo(const InetSocketAddress &addr, const RawMessage &msg) const
{
    return _socket.sendTo(addr, msg._buffer.bytes(), msg._buffer.length());
}

// TODO clean this up
int IcmpSocket::recvFrom(ICMP::RawMessage &msg, int timeout)
{
	//SRL::float64 wait_timer = System::GetSystemTimer();
	// loop until we have the packet or we have timedout
	if (!_socket.isRecvReady(timeout))
	{
        return 0;
		//throw Errors::SocketTimeoutException("Timed Out waiting to receive ICMP Packet");
	}
	
	// mark the receive time on the packet
	msg.markReceived();
	// need a temp variable  to pass size pointer
	//socklen_t tmp_size =  sizeof(sockaddr);


    int received = 0;
	// receive the data
    try
    {
        received = _socket.recvFrom(msg._from, msg._buffer.raw(), msg._buffer.size(), 0);
    }
    catch(Net::Errors::SocketException &e)
    {
            //         if (System::GetLastErrorNumber() == EINTR)
            //         {
            //              // shrink the wait time
            // timeout -= (System::GetSystemTimer() - wait_timer) * 1000.0;
            // if (timeout <= 0)
            //  timeout = 1;
            // return recvFrom(msg, timeout);           
            //         }
        throw e;
    }

	msg._buffer.setLength(received);

	// FIXME this needs to be some generic header that looks for the type
	msg.sync(false);
	return received;
}

void IcmpSocket::sendEchoRequest(const InetSocketAddress &to_addr, const uint32 &sequence, SRL::uint16 msg_size, SRL::uint16 ttl)
{
    // TODO ICMP WITH MORE THEN ONE THREAD WILL BREAK ON APPLE
	if (_uid == 0)
	{
	    #ifdef __APPLE__
	        _uid = System::GetPid();
	    #else
		    _uid = System::GetCurrentThreadID();
		#endif
	}
	
	if (ttl > 0)
	{
	    _socket.setTimeToLive(ttl);
        //printf("ttl=%d\n", ttl);
	}
	
	_echo_request->update(_uid, sequence, msg_size);
	// update the reply buffer to handle the new size
	if (_echo_reply->size() < msg_size)
		_echo_reply->resize(msg_size+IP::HEADER_SIZE);
	
	this->sendTo(to_addr, *_echo_request);
}

// clean this up
ICMP::Response IcmpSocket::receiveEchoReply(int timeout)
{
    //SRL::float64 sent_timer = System::GetSystemTimer();
    SRL::float64 timer = 0.0;
    SRL::float64 elapsed = 0.0;
	do
	{
        timer = System::GetSystemTimer();
		if (recvFrom(*_echo_reply, timeout) > 0)
        {
    		if (_echo_reply->id() == _uid)
    		{
    		    if (_echo_reply->_rtt < 0.0)
    		    {
    		        elapsed = (System::GetSystemTimer() - timer);
    		        _echo_reply->_rtt = static_cast<int>(elapsed * 1000.0);
    			}
    			return *_echo_reply;
    		}
        }        
        elapsed = (System::GetSystemTimer() - timer);
        timeout = timeout - static_cast<int>(elapsed * 1000.0);
        //printf("elapsed=%0.2f, timeout=%d\n", elapsed, timeout);
	}
	while (timeout > 0);
	
	throw Errors::SocketTimeoutException("Timed Out waiting to receive ICMP Packet");
}

void IcmpSocket::startListener(IcmpSocket::Listener *listener)
{
	if (this->isAlive())
		throw SRL::Errors::ThreadException("Listener is already running!");

	_listener = listener;
	this->start();
}

void IcmpSocket::stopListener()
{
	if (this->isAlive())
		this->stop();
}

bool IcmpSocket::initial()
{
	return true;
}

void IcmpSocket::final()
{

}

bool IcmpSocket::run()
{
	// now lets read all the incoming data
	while (true)
	{
		doStateCheck();
		try
		{
			recvFrom(*_echo_reply, 1000);
			// TODO implement more types
			switch (_echo_reply->type())
			{
				case (Message::ECHO_REPLY):
					_listener->icmp_echo_reply(*_echo_reply);
					break;
				default:
					_listener->icmp_unknown(*_echo_reply);
			}
		}
		catch (SocketException &e)
		{
			// ignore and continue on
		}
	}
}

