#include "srl/net/ServerSocket.h"

using namespace SRL;
using namespace SRL::Net;

ServerSocket::ServerSocket(const int& port) : 
_socket(Socket::TCP), _listener(NULL)
{
	InetSocketAddress addr(port);
	_socket.bind(addr);
}

ServerSocket::ServerSocket(const InetSocketAddress& addr) : 
_socket(Socket::TCP), _listener(NULL)
{
	_socket.bind(addr);
}

ServerSocket::~ServerSocket()
{
	_socket.close();
}

Socket::Descriptor ServerSocket::accept()
{
    return _socket.accept();    
}

void ServerSocket::startListener(EventListener *listener)
{
    _listener = listener;
    System::Thread::start();
}

void ServerSocket::stopListener()
{
    System::Thread::stop(true);
}

void ServerSocket::waitForRecv(const int32& msec)
{
    // only if we have a listener
    if (_listener == NULL)
        return;
        
	int result=SRL::INVALID; fd_set recv_set, error_set;
	FD_ZERO(&recv_set); FD_ZERO(&error_set); 
	int high_desc = 0;
	for( uint32 i = 0; i < _connections.size(); ++i )
	{
	    FD_SET(_connections[i].raw(), &recv_set );
	    FD_SET(_connections[i].raw(), &error_set );
	    if (_connections[i].raw() > high_desc)
	        high_desc = _connections[i].raw();
	}
	
	if( msec == SRL::FOREVER )
		result = ::select( high_desc+1, &recv_set, 0, &error_set, 0 );
	else 
	{
		timeval tv; tv.tv_sec = msec/1000; tv.tv_usec = (msec*1000)%1000000;
		result = ::select( high_desc+1, &recv_set, 0, &error_set, &tv );
	}
	// check for errors
	if (result == -1)
		throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
    
    if (result > 0)
    {
        Util::Vector<Socket::Descriptor>::Iterator iter = _connections.begin();
        while (iter != _connections.end())
    	{
    	    if (FD_ISSET(iter.value().raw(), &error_set))
    	    {
    	        _listener->connection_closed(iter.value());
    	        iter = _connections.erase(iter);
    	        continue;	    
    	    }
    	    
    	    if (FD_ISSET(iter.value().raw(), &recv_set))
    	        _listener->connection_recvReady(iter.value());
    	    ++iter;
	    }
    }
    
	return;
}


bool ServerSocket::initial()
{
    listen();
    return true;
}

bool ServerSocket::run()
{
    while (true)
    {
        Socket::Descriptor desc = _socket.accept();
        _connections.add(desc);
		if (_listener != NULL)
		{
			_listener->connection_new(desc);
		}

        doStateCheck(); 
    }
    return true;
}

void ServerSocket::final()
{
    for( uint32 i = 0; i < _connections.size(); ++i )
    {
        _connections[i].close();
    }
    _connections.clear();
}

