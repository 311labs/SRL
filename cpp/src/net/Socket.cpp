
#include "srl/net/Socket.h"
#include "srl/sys/System.h"
#include <signal.h>
#include <fcntl.h>
#ifndef WIN32
#include <netinet/tcp.h>
#endif

#include "srl/Console.h"

using namespace SRL;
using namespace SRL::Net;
using namespace SRL::Net::Errors;

// -------- BEGIN SOCKET DESCRIPTOR ------------------

Socket::Descriptor::Descriptor() : 
_socket(SRL::INVALID), 
_stype(Socket::INVALID_TYPE), 
_connected(false),
_bound(false),
_listening(false),
_isblocking(true),
_refcnt(0)
{
}

Socket::Descriptor::Descriptor(SOCKET s, Socket::TYPE st) : 
_socket(s), 
_stype(st), 
_connected(false),
_bound(false),
_listening(false),
_isblocking(true),
_refcnt(0)
{
} 
  
Socket::Descriptor::Descriptor(const Descriptor &desc) : 
_socket(desc._socket), 
_stype(desc._stype), 
_connected(desc._connected),
_bound(desc._bound),
_listening(desc._listening),
_isblocking(desc._isblocking),
_refcnt(0)
{

} 
 
Socket::Descriptor& Socket::Descriptor::operator=(const Socket::Descriptor& desc)
{
    _socket = desc._socket;
    _stype = desc._stype;
    _connected = desc._connected;
    _bound = desc._bound;
    _listening = desc._listening;
    _isblocking = desc._isblocking;
    //printf("socket copy: %d to %d\n", desc._socket, _socket);
    return *this;
} 
 
void Socket::Descriptor::open(Socket::TYPE st)
{
    if (isValid())
        throw SocketException("socket open failed, already have valid socket handle");
    _stype = st;
    const SocketTypeDef &sd = Socket::GetSocketDef(st);
    _connected = (st == Socket::UDP)||(st == Socket::ICMP)||(st == Socket::RAW);
#ifdef WIN32
    _socket = ::WSASocket(sd.domain, sd.type, sd.protocol, NULL,0,WSA_FLAG_OVERLAPPED);
#else
    _socket = ::socket(sd.domain, sd.type, sd.protocol);
    // lets close the socket on exec
    fcntl(_socket, F_SETFD, FD_CLOEXEC);
#endif
    if (!isValid())
    {
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) ); 
    }
}
 
void Socket::Descriptor::close()
{
    if (_socket != SRL::INVALID)
    {   
        ::closesocket(_socket );
        _socket = SRL::INVALID;
    }
    _connected = false; _bound=false; _listening=false;
}
 
void Socket::Descriptor::reset()
{
    close();
    open(_stype);
}


// -------- END SOCKET DESCRIPTOR --------------------
// FIXME descriptor needs to be newed so that we can do reference counting
Socket::Socket(Socket::TYPE st) :
_desc(SRL::INVALID, st)
{
    Initialize();
    _desc.open(st);
}

Socket::Socket(const Socket &socket) : 
_desc(socket._desc)
{

}

Socket& Socket::operator=(const Socket& s)
{
    _desc = s._desc;
    return *this;
}

Socket::Socket(const Descriptor &desc) : 
_desc(desc)
{

}

Socket::~Socket()
{
    close();
}

void Socket::setBlockingMode(bool blocking)
{
    if (blocking == _desc.isBlockingMode())
        return;
#if defined(WIN32) 
    u_long FAR value = 1; 
    if (blocking)
        value = 0;
    if( ioctlsocket( _desc._socket, FIONBIO, &value ) != 0 )
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );  
#else 
    long arg = ::fcntl(_desc._socket, F_GETFL, NULL); 
    if (arg == -1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
        
    if (blocking)
        arg &= (~O_NONBLOCK);
    else
        arg |= O_NONBLOCK; 
        
    if (::fcntl(_desc._socket, F_SETFL, arg) == -1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
#endif
    _desc._isblocking = blocking;
}

bool Socket::isBlockingMode() const
{
    return _desc.isBlockingMode();
}

void Socket::connect(const InetSocketAddress &endpoint, int timeout)
{
    if (_desc.inUse())
        throw SocketException("socket is already in use! can not connect");
    
    const sockaddr* addr = (sockaddr*)(sockaddr_in*)&endpoint._addr;
    if (timeout == FOREVER)
    {
        if (::connect( _desc._socket, addr, sizeof(endpoint._addr)) == SRL::INVALID)
            throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
        _desc._connected  = true;
    }
    else
    {
        bool blocking_mode = isBlockingMode();
        setBlockingMode(false);
        ::connect( _desc._socket, addr, sizeof(endpoint._addr));
        if (!isSendReady(timeout))
        {
            throw Errors::SocketTimeoutException("timed out during connect call");
        }
        setBlockingMode(blocking_mode);
    }
}


void Socket::connect(const String& hostname, const int& port, int timeout)
{
    InetSocketAddress addr(hostname, port);
    connect(addr, timeout);
}

void Socket::bind(const InetSocketAddress &endpoint, bool reuse)
{
    if (_desc.inUse())
        throw SocketException("socket is already in use! can not be bound");
    
    if (reuse)
    {
        int flag = 0;
        if (reuse)
            flag = 1;
        if (::setsockopt(_desc._socket, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(flag)) == -1)
            throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );      
    }
    
    sockaddr* addr = (sockaddr*)&endpoint._addr;    
    if (::bind( _desc._socket, addr, sizeof(endpoint._addr)) == SRL::INVALID)
    {
        throw Errors::SocketException( String::Format("socket bind(%s, %d) failed: %s", 
            endpoint.getAddress().getHostAddress().text(), endpoint.getPort(),
            System::GetErrorString(System::GetLastErrorNumber())) );
    }
    _desc._bound = true;
}

void Socket::bind(const int& port, bool reuse)
{
    InetSocketAddress addr(port);
    bind(addr, reuse);
}

void Socket::bind(const String& hostname, const int& port, bool reuse)
{
    InetSocketAddress addr(hostname, port);
    bind(addr, reuse);
}

void Socket::listen(int maxcons)
{
    if (!_desc.isBound())
        throw SocketException("socket must be bound first");
    if (::listen( _desc._socket, maxcons ) == SRL::INVALID)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
    _desc._listening = true;
}

Socket::Descriptor Socket::accept()
{
    Descriptor newdc(::accept( _desc._socket, NULL, NULL ), _desc._stype);
    if (!newdc.isValid())
        throw SocketException("accept failed");
        
    newdc._connected = true;
#ifndef WIN32
    fcntl(newdc._socket, F_SETFD, FD_CLOEXEC);
#endif
    return newdc;
}

/** close the socket (disconnect if connected) */
void Socket::close()
{
    _desc.close();
}

void Socket::shutdown(Socket::SHUTHOW sh)
{
    if (::shutdown( _desc._socket, sh ) != SRL::INVALID)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
}

bool Socket::isSendReady(int msec)
{
    // setup our variables
    int result=SRL::INVALID; fd_set files;
    FD_ZERO(&files); FD_SET( _desc._socket, &files );
    
    if( msec == SRL::FOREVER )
        result = ::select( _desc._socket+1, 0, &files, 0, 0 );
    else 
    {
        timeval tv; tv.tv_sec = msec/1000; tv.tv_usec = (msec*1000)%1000000;
        result = ::select( _desc._socket+1, 0, &files, 0, &tv );
    }
    // check for errors
    if (result == -1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );

    return result && (result != SRL::INVALID);    
}

bool Socket::isRecvReady(int msec)
{
    // setup our variables
    int result=SRL::INVALID; fd_set files;
    FD_ZERO(&files); FD_SET( _desc._socket, &files );
    
    if( msec == SRL::FOREVER )
        result = ::select( _desc._socket+1, &files, 0, 0, 0 );
    else 
    {
        timeval tv; tv.tv_sec = msec/1000; tv.tv_usec = (msec*1000)%1000000;
        result = ::select( _desc._socket+1, &files, 0, 0, &tv );
    }
    // check for errors
    if (result == -1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );

    return result && (result != SRL::INVALID);    
}

bool Socket::isExceptionPending(int msec)
{
    // setup our variables
    int result=SRL::INVALID; fd_set files;
    FD_ZERO(&files); FD_SET( _desc._socket, &files );
    
    if( msec == SRL::FOREVER )
        result = ::select( _desc._socket+1, 0, 0, &files, 0 );
    else 
    {
        timeval tv; tv.tv_sec = msec/1000; tv.tv_usec = (msec*1000)%1000000;
        result = ::select( _desc._socket+1, 0, 0, &files, &tv );
    }
    // check for errors
    if (result == -1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );

    return result && (result != SRL::INVALID);    
}

const InetSocketAddress& Socket::getRemoteAddress() const
{
    if (!_remote_addr.isValid())
    {
        // lets fill the remote address sockaddr property 
        socklen_t len = sizeof(_remote_addr._addr);     
        if (::getpeername( _desc._socket, (sockaddr*)&_remote_addr._addr, &len ) == SRL::INVALID)
        {
            throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
        }
        // copy the 
        _remote_addr._inet_addr._addr.s_addr = _remote_addr._addr.sin_addr.s_addr;
        _remote_addr._port = ntohs(_remote_addr._addr.sin_port);
    }
    return _remote_addr;
}

const InetSocketAddress& Socket::getLocalAddress() const
{
    if (!_local_addr.isValid())
    {
        // lets fill the remote addr
        sockaddr* addr = (sockaddr*)&_local_addr._addr;
        socklen_t len = sizeof(sockaddr);
        if (::getsockname( _desc._socket, addr, &len ) == SRL::INVALID)
            throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );

        _local_addr._inet_addr._addr.s_addr = _local_addr._addr.sin_addr.s_addr;
        _local_addr._port = ntohs(_local_addr._addr.sin_port);
    }
    return _local_addr;
}




uint8 Socket::getTimeToLive() const
{
    uint8 result;
    socklen_t rsize=sizeof(uint8);
    if (::getsockopt(_desc._socket, IPPROTO_IP, IP_TTL, (char*)&result, &rsize)==-1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
    return result;
}

void Socket::setTimeToLive(int ttl)
{
#ifdef WIN32

    if (::setsockopt(_desc._socket, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl)) == -1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );    
#else
    if (::setsockopt(_desc._socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );   
#endif
}

void Socket::setKeepAlive(bool flag)
{
    if (::setsockopt(_desc._socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag, sizeof(bool)) == -1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
}

void Socket::setBroadcastFlag(bool flag)
{
    if (::setsockopt(_desc._socket, SOL_SOCKET, SO_BROADCAST, (char*)&flag, sizeof(bool)) == -1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
}

void Socket::setNagleFlag(bool flag)
{
    flag = !flag;
    if (::setsockopt(_desc._socket, SOL_SOCKET, TCP_NODELAY, (char*)&flag, sizeof(bool)) == -1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
}


void Socket::setReceiveBufferSize(const int& recv_size)
{
    if (::setsockopt(_desc._socket,SOL_SOCKET, SO_RCVBUF, (char *)&recv_size, sizeof(recv_size)) == -1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
}

void Socket::setSendBufferSize(const int& send_size)
{
    if (::setsockopt(_desc._socket,SOL_SOCKET, SO_SNDBUF, (char *)&send_size, sizeof(send_size)) == -1)
        throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
}

// ------- BEGIN IO FUNCTIONS ---------
int Socket::send(const SRL::byte* data, int len)
{
    // TODO MSG_NOSIGNAL will cause the socket not to send a SIGPIPE on errors
    if (!isConnected())
        throw SocketException("socket must be connected to send data");
        
    int senttotal = 0;
    //printf("socket sending %d bytes\n", len);
    byte *unsentbuf = (byte*)data;
        
    while (len)
    {
        int sent = ::send(_desc._socket, (const char*)unsentbuf, len*sizeof(char), 0);
        // if non are sent then the socket is disconnected
        if (sent == 0)
        {
            close();
            throw SocketDisconnected("socket disconnected during send");
        }
        else if (sent < 0)
        {
            throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
        }
        unsentbuf += sent; senttotal += sent; len -= sent;
    }
    
    return senttotal;
}

int Socket::send(const String& msg)
{
    //printf("socket sending string that is %u bytes\n", msg.size());
    return send((byte*)msg.text(), msg.size());
}


int Socket::recv(SRL::byte* data, int len, int msec)
{
    if (!isConnected())
        throw SocketException("socket must be connected to receive data");

    int received = 0;

    if ((msec == SRL::FOREVER) || (isRecvReady(msec)))
    {
        received = ::recv(_desc._socket, (char*)data, len*sizeof(byte), 0);
        //printf("socket ::recv %d bytes\n", received);
        // if nothing is received then the socket is disconnected
        if (received == 0)
        {
            close();
            throw SocketDisconnected("socket disconnected during receive");
        }
        else if (received == SRL::INVALID)
        {
            throw Errors::SocketException( System::GetErrorString(System::GetLastErrorNumber()) );
        }
    }
    return received;
}

int Socket::recv(String &buf, int len, int msec)
{
    buf.resize(len);
    int received = recv((byte*)buf._strPtr(), len*sizeof(char), msec);
    buf.resize(received);
    return received;
}

int Socket::sendTo(const InetSocketAddress &to_addr, const SRL::byte *data, uint32 len, uint32 flags) const 
{
    int res = ::sendto(_desc._socket, (char*)data, len, flags, 
                        (sockaddr*)&to_addr._addr, sizeof(sockaddr));
    if (res == -1)
        throw Errors::SocketException( String::Format("Socket sendto(%s, %d) failed: %s", 
            to_addr.getAddress().getHostAddress().text(), to_addr.getPort(),
            System::GetErrorString(System::GetLastErrorNumber())) );
    if (res != static_cast<int32>(len))
        throw SocketException("failed to send data");
    return res;
}

int Socket::recvFrom(InetSocketAddress &from_addr, SRL::byte *buffer, uint32 len, uint32 flags) const
{
    socklen_t tmp_size = sizeof(sockaddr);
    int res = ::recvfrom(_desc._socket, (char*)buffer, len, flags,
                         (sockaddr*)&from_addr._addr, &tmp_size);
    if (res == -1)
        throw Errors::SocketException( String::Format("Socket recvfrom failed: %s", 
            System::GetErrorString(System::GetLastErrorNumber())) );
    buffer[res] = '\0';
    from_addr._inet_addr._addr.s_addr = from_addr._addr.sin_addr.s_addr;
    from_addr._port = ntohs(from_addr._addr.sin_port);
    return res;
}


// ------- END IO FUNCTIOSN ----------

// Stole this from another Socket Class don't like it but it works
Socket::SocketTypeDef& Socket::GetSocketDef( Socket::TYPE ty)
{
    static Socket::SocketTypeDef typeTable[] = 
    {
        {SRL::INVALID, 0,           0           },
        {PF_INET, SOCK_STREAM,  IPPROTO_TCP },
        {PF_INET, SOCK_DGRAM,   IPPROTO_UDP },
        {PF_INET, SOCK_RAW,     IPPROTO_ICMP},
        {PF_INET, SOCK_RAW,     IPPROTO_RAW },
        {PF_UNIX, SOCK_STREAM,  0           },
        {PF_UNIX, SOCK_DGRAM,   0           }
    };
    
    if( ty < Socket::INVALID_TYPE || ty >= static_cast<int32>(sizeof(typeTable)/sizeof(SocketTypeDef)) )
        ty = Socket::INVALID_TYPE;
        
    return typeTable[ty];
}




