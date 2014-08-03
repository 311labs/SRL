/*********************************************************************************
* Copyright (C) 1999,2006 by Ian C. Starnes   All Rights Reserved.        
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*******************************************************************************/

#ifndef __SRL_Socket__
#define __SRL_Socket__

#include "srl/net/InetSocketAddress.h"

namespace SRL
{
    namespace Net
    {
        /** a socket is used to communicate data over a network from one endpoint to another */
        class SRL_API Socket
        {
        public:
            // Type of Socket Stream
            enum TYPE {
                INVALID_TYPE,
                INET_STREAM,
                TCP = INET_STREAM,
                INET_DGRAM,
                UDP = INET_DGRAM,
                INET_ICMP,
                ICMP = INET_ICMP,
                INET_RAW,
                RAW = INET_RAW,
                UNIX_STREAM, 
                UNIX_DGRAM 
            };
            
            // what to shutdown on the socket stream
            enum SHUTHOW {
                SHUT_READ,
                SHUT_WRITE,
                SHUT_BOTH
            };

            /** Structure that holds the socket descriptor and referencecount */
            class SRL_API Descriptor
            {
            friend class Socket;
            protected:
                /** handle to the socket */
                SOCKET _socket;
                /** type of socket */
                Socket::TYPE _stype;
                /** is the current socket connected */
                bool _connected, _bound, _listening, _isblocking;
                /** share counter */
                int _refcnt;
            public:
                /** basic contructor */
                Descriptor();
                /** copy contructor */
                Descriptor(const Descriptor &desc);
                /** assignment copy */
                Descriptor& operator=(const Descriptor& desc);
                /** the default contructor */
                explicit Descriptor( SOCKET s, Socket::TYPE st);
                /** is the current socket handle valid */
                bool isValid() const{ return (_socket != SRL::INVALID);};   
                /**  is the current socket handle connected */
                bool isConnected() const{ return _connected;};
                /** is the current socket handle bound */
                bool isBound() const{ return _bound;};
                /** is the current socket handle listening */
                bool isListening() const{ return _listening;};
                /** is the current socket in use (listening or bound) */
                bool inUse() const{ return _listening || _bound;};
                /** is blocking mode */
                bool isBlockingMode() const{ return _isblocking; }
                /** open socket */
                void open(Socket::TYPE st);
                /** close socket */
                void close();
                /** reset the socket */
                void reset();
                /** return the raw descriptor */
                SOCKET raw() const{return _socket;}
                /** compare our String to std string */
                friend SRL_API bool operator==(const Descriptor& s1,const Descriptor& s2)
                    { return (s1._socket == s2._socket);}
                
            };

            /** Creates an unconnected socket*/
            Socket(Socket::TYPE st=TCP);
            /** create a new socket with an existing socket descriptor, used with accept */
            Socket(const Socket::Descriptor &desc);
            
            /** Destructor */
            virtual ~Socket();

            /** copy constructor */
            Socket(const Socket &socket);
            /** Assign another string to this */
            Socket& operator=(const Socket& s);
            
            /** Connects this socket to the ip/port pairing in the InetSocketAddress */
            void connect(const InetSocketAddress &endpoint, int timeout=FOREVER);           
            /** Connects this socket to the hostname/port */
            void connect(const String& hostname, const int& port, int timeout=FOREVER);

            /** bind this socket to the any /port */
            void bind(const int& port, bool reuse=false);
            /** bind this socket to the local ip/port pairing in the InetSocketAddress */
            void bind(const InetSocketAddress &endpoint, bool reuse=false);         
            /** bind this socket to the hostaddress/port */
            void bind(const String& hostaddress, const int& port, bool reuse=false);            
            
            /** listen for connections on this socket */
            void listen(int maxcons=SOMAXCONN);

            /** Accept a connection on the socket (return a socket descriptor) */
            Socket::Descriptor accept();

            /** is the current socket handle valid */
            bool isValid(){ return _desc.isValid();};
            /** Checks if the current socket is connected to a remote socket */
            bool inUse(){ return _desc.inUse();};
            /** Checks if the current socket is connected to a remote socket */
            bool isConnected(){ return _desc.isConnected();};
            /** Checks if the current socket is connected to a remote socket */
            bool isBound(){ return _desc.isBound();};

            /** Returns the current Send state of the socket */
            bool isSendReady(int msec=SRL::NOW);
            /** Returns the current Recv state of the socket */
            bool isRecvReady(int msec=SRL::NOW);
            /** Checks if any exceptions are pending */
            bool isExceptionPending(int msec=SRL::NOW);
            
            /** sends data accross the socket. returns the number of bytes sent */
            int send(const SRL::byte* data, int len);
            /** sends data as const char* */
            int send(const char* data, int len){ return send((byte*)data,len);};
            /** Send the following string */
            int send(const String& msg);          
            
            /** receive data from the socket. returns  the number of bytes received */
            int recv(SRL::byte* data, int len,  int msec=FOREVER);          
            /** receive data as const char* */
            int recv(char* data, int len, int msec=FOREVER){ return recv((byte*)data,len,msec);};
            /** receive len number of bytes into buf, returns the amount read */
            int recv(String &buf, int len, int msec=FOREVER);
            
            /** send a datagram(UDP Packet) */
            int sendTo(const InetSocketAddress &to_addr, const SRL::byte *data, uint32 len, uint32 flags=0) const;
            /** receive a datagram */
            int recvFrom(InetSocketAddress &from_addr, SRL::byte *buffer, uint32 len, uint32 flags=0) const;            
            
            /** returns remote address */
            const InetSocketAddress &getRemoteAddress() const;
            /** returns the local address */
            const InetSocketAddress &getLocalAddress() const;
            
            /** returns the socket descriptor */
            const Socket::Descriptor& getDescriptor() const{ return _desc; }
            
            /** reset the socket so it can be used again */
            void reset(){ _desc.reset(); }
            /** Closes the socket. */
            void close();
            /** shutdown io */
            void shutdown(Socket::SHUTHOW sh);

            /** set blocking mode */
            void setBlockingMode(bool blocking);
            /** returns true if in blocking mode */
            bool isBlockingMode() const;

            /** changes the size of the receive buffer at the kernel level */
            void setReceiveBufferSize(const int& recv_size);
            /** changes the size of the send buffer at the kernel level */
            void setSendBufferSize(const int& send_size);
        
            /** set the keepalive option */
            void setKeepAlive(bool flag);
            /** set broadcast option */
            void setBroadcastFlag(bool flag);
            /** set tcpno delay option (toggle nagle algorithm on by default) */
            void setNagleFlag(bool flag);
  
            /** Retrieve the Time to Live option */
            uint8 getTimeToLive() const;
            /** @short Sets the Time to Live option
            *
            * Time to live tells the routes how far to allow this packet to travel before dropping it
            */
            void setTimeToLive(int ttl);  
            
        private:
            /** Socket Descriptor */
            Descriptor _desc;
            /** the remote address */
            mutable InetSocketAddress _remote_addr;
            /** the local address */
            mutable InetSocketAddress _local_addr;  
        protected:
            /** Structure that holds the socket stream definitions */
            struct SocketTypeDef {
                int domain, type, protocol;
            };
            /** returns the current socket parameters for the given socket type */
            static SocketTypeDef& GetSocketDef( Socket::TYPE i );            
        };
    }
}

#endif // __SRL_Socket__

