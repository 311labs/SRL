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
#ifndef __SRL_NetworkAddress__
#define __SRL_NetworkAddress__

#include "srl/String.h"
#include "srl/util/Vector.h"
#include "srl/Exceptions.h"
#include "srl/sys/Mutex.h"

#ifdef WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <sys/types.h>
	#define SIGPIPE SIGABRT // windows does not generate SIGPIPE
#else
    #define BSD_COMP
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <sys/unistd.h>
    #define SOCKET int
    #define SOCKET_ERROR -1
    #define closesocket close

    #ifdef _Linux__
        #define MSG_MAXIOVLEN    16
        #define SOMAXCONN    5
    #endif // _Linux__
#endif


namespace SRL
{      
    namespace Net
    {       
        namespace Errors
        {
            /** Base for all Socket IOExceptions */
            class SRL_API SocketException : public SRL::Errors::IOException
            {
            public:
                /** default constructor */
                SocketException(String message): SRL::Errors::IOException(message){};
            };

            /** Thrown when the socket disconnects */
            class SRL_API SocketDisconnected : public SocketException
            {
            public:
                /** default constructor */
                SocketDisconnected(String message): SocketException(message){};
            };

            /** Thrown when the host can not be resolved or is invalid */
            class SRL_API UnknownHostException : public SocketException
            {
            public:
                /** default constructor */
                UnknownHostException(String message): SocketException(message){};
            };

            /** Thrown when the socket operation has timed out */
            class SRL_API SocketTimeoutException : public SocketException
            {
            public:
                /** default constructor */
                SocketTimeoutException(String message): SocketException(message){};
            };

            /** Thrown when the host address is unreachable because no route can be found */
            class SRL_API NoRouteToHostException : public SocketException
            {
            public:
                /** default constructor */
                NoRouteToHostException(String message): SocketException(message){};
            };

            /** Thrown for invalid addresses */
            class SRL_API InvalidAddressException : public SocketException
            {
            public:
                /** default constructor */
                InvalidAddressException(String message): SocketException(message){};
            };

            /** Thrown for destination unreachable */
            class SRL_API DestinationUnreachable : public SocketException
            {
            public:
                /** default constructor */
                DestinationUnreachable(String message): SocketException(message){};
            };		

		}

        /** dumby for socket class */
        class Socket;
        /** This class represents an Internet Protocol (IP) address.
        
        An IP address is either a 32-bit or 128-bit unsigned number used by IP, a lower-level protocol on which protocols
        like UDP and TCP are built. 
        */
        class SRL_API InetAddress
        {
        friend class InetSocketAddress;
        friend class Socket;
        friend class DatagramSocket;
        friend class MulticastSocket;
        public:
            /** returns a vector of InetAddress's by hostname (both the vector and the InetAddresses are newed and you are
            responsible for deleting them) */
            Util::Vector<InetAddress*>* GetAllAByName(const String &name);
    
        public:
            /** constructor (INVALID ADDRESS CREATED) */
            InetAddress();       
            /** Creates an InetAddress object given the raw IP address */
            InetAddress(in_addr &addr);
            /** Creates an InetAddress given the host */
            InetAddress(const String& host);
            /** copy constructor */
            InetAddress(const InetAddress& addr);
            /** destructor */
            virtual ~InetAddress();
            friend bool operator==(const InetAddress& s1,const InetAddress& s2){ return (s1._addr.s_addr == s2._addr.s_addr); };
            /** returns the raw ip address */
            const SRL::byte* getAddress() const;
            /** returns a string representing the ip */
            const String& getHostAddress() const;
            /** returns a string representing the hostname */
            const String& getHostName() const;
            /** Gets the fully string2ualified domain name for this IP address */
            const String& getCanonicalHostName() const;
            
            /** return true if this is a valid address */
            bool isValid() const;

            /** this is the data type that the lowlevel sockets use */
            in_addr _addr;

        protected:        

            /** the ip address as a string */
            mutable String _ip;
            /** the hostname as a string */
            mutable String _hostname;
            /** the fully qualified name as a string */
            mutable String _domain_name;
        };

        
        /** stupid winsock initialization used before doing socket api calls */
        extern "C"
        {
            inline int Initialize()
            {
            #ifdef WIN32
                WSADATA d;
                return ::WSAStartup( MAKEWORD( 2, 2 ), &d );
            #else
                return 0;
            #endif    
            }
        }
    }
}

#endif // __SRL_NetworkAddress__


