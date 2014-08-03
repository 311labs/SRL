#include "srl/net/TcpSocket.h"
#include "srl/sys/System.h"

using namespace SRL;
using namespace SRL::Net;
using namespace SRL::IO;

TcpSocket::TcpSocket() :
Socket(),
_buffer(1024),
_max_size(512000)
{
    
}


TcpSocket::TcpSocket(const Socket::Descriptor& desc) :
Socket(desc),
_buffer(1024),
_max_size(512000)
{
}

/** Returns there is unread data in the local buffer or on the socket */
bool TcpSocket::isReadReady(int msec)
{
    if (_buffer.remaining() > 0)
        return true;
    return isRecvReady(msec);
}

uint32 TcpSocket::readUntil(String& buf, const char& c, int msec)
{
    if (_buffer.readUntil(buf, c) > 0)
    {
        return buf.length();
    }
    else
    {
        // no match in the byte buffer... lets try the socket
        // socket NOW logic
        if (msec == SRL::NOW)
        {
            _bufferedRecv();
            return _buffer.readUntil(buf, c);
        }
        // socket FOREVER logic
        while (msec == SRL::FOREVER)
        {
            if (isRecvReady(SRL::FOREVER))
            {
                _bufferedRecv();
                if (_buffer.readUntil(buf, c) > 0)
                {
                    return buf.length();
                }
            }
        }
        
        // socket timer logic
        uint32 timer = System::GetSystemTimer();
        while ((msec > 0) && isRecvReady(msec))
        {
            _bufferedRecv();
            if (_buffer.readUntil(buf, c) > 0)
            {
                return buf.length();
            }
            uint32 ntimer = System::GetSystemTimer();
            msec = msec - (int)(ntimer - timer);
        }
    }
    return 0;
}

uint32 TcpSocket::readUntil(String& buf, const String& str, int msec)
{
    if (_buffer.readUntil(buf, str) > 0)
    {
        return buf.length();
    }
    else
    {
        // no match in the byte buffer... lets try the socket
        // socket NOW logic
        if (msec == SRL::NOW)
        {
            _bufferedRecv();
            return _buffer.readUntil(buf, str);
        }
        // socket FOREVER logic
        while (msec == SRL::FOREVER)
        {
            if (isRecvReady(SRL::FOREVER))
            {
                _bufferedRecv();
                if (_buffer.readUntil(buf, str) > 0)
                {
                    return buf.length();
                }
            }
        }
        
        // socket timer logic
        uint32 timer = System::GetSystemTimer();
        while ((msec > 0) && isRecvReady(msec))
        {
            _bufferedRecv();
            if (_buffer.readUntil(buf, str) > 0)
            {
                return buf.length();
            }
            uint32 ntimer = System::GetSystemTimer();
            msec = msec - (int)(ntimer - timer);
        }
        
    }
    return 0;
}

uint32 TcpSocket::readAll(String& output, const uint32& len)
{
    //printf("reading all: %u bytes\n", len);
    while(_buffer.remaining() < len)
    {
        if (!isRecvReady(10000))
            throw SRL::Errors::IOException("timed out waiting for readAll to complete");
        _bufferedRecv();
    //printf("pos: %u  remaining: %u  length: %u\n", _buffer.position(), _buffer.remaining(), _buffer.length());
    }
    return _buffer.read(output, static_cast<uint32>(len)-1);
} 

/** receive data from the socket. returns  the number of bytes received */
int TcpSocket::read(SRL::byte* data, int len,  int msec)
{
    if (_buffer.remaining() > 0)
    {
        return _buffer.read(data, len);
    }
    
    return Socket::recv(data, len, msec);
}   

/** receive len number of bytes into buf, returns the amount read */
int TcpSocket::read(String &buf, int len, int msec)
{
    if (_buffer.remaining() > 0)
    {
        return _buffer.read(buf, (uint32)len);
    }
    
    return Socket::recv(buf, len, msec);
}

int TcpSocket::_bufferedRecv()
{
    //printf("before [byte buffer] pos=%d length=%d\n", _buffer.position(), _buffer.length());
    _buffer.toggle();
    //printf("after [byte buffer] pos=%d length=%d\n", _buffer.position(), _buffer.length());
    // check if we need to resize the buffer
    uint32 free_bytes = _buffer.size() - _buffer.position();
    //first check for no free buffer
    if ((free_bytes == 0) && (_buffer.size() >= _max_size))
    {
        throw SRL::Errors::OutOfMemoryException("yeah have reached the maximum allowed memory (try increasing the TcpSocket::max_size)");
    }
    else if (free_bytes <= 256)
    {
        // check if max size has been reached
        uint32 new_size = _buffer.size() + 1024;
        if (new_size > _max_size)
            new_size = _max_size;
        _buffer.resize(new_size);
        free_bytes = _buffer.size() - _buffer.position();
    }
    
    int bytes_read = Socket::recv(&_buffer[_buffer.position()], free_bytes, NOW);
    //printf("\tSocket received %d bytes\n", bytes_read);
    if (bytes_read > 0)
    {
        _buffer.setLength(bytes_read + _buffer.position());
        _buffer.setPosition(bytes_read+_buffer.position());
    }
    _buffer.toggle(false);
    //printf("\n==== begin buffer\n%s\n==== end buffer\n\n", _buffer.c_str());
    //printf("ready [byte buffer] pos=%d length=%d\n", _buffer.position(), _buffer.length());
    return bytes_read;    
}
