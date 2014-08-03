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
#ifndef __SRL_SocketAddress__
#define __SRL_SocketAddress__

#include "srl/String.h"
#include "srl/net/InetAddress.h"

namespace SRL
{
    namespace Net
    {
        /**
        This class represents a pairing of an IP Address and a port number used by sockets 
        A valid port value is between 0 and 65535.
        */
        class SRL_API InetSocketAddress
        {
        friend class Socket;
        friend class DatagramSocket;
        friend class MulticastSocket;
        public:
            /** constructor */
    		InetSocketAddress();
    		/** Creates a socket address from an IP address and a port number. */
    		InetSocketAddress(const InetAddress &addr, const int& port);
    		/** Creates a socket address from a hostname and port number. */
    		InetSocketAddress(const String& hostname, const int port=0);  
            /** Creates a socket address with the ip being the wild card address 0.0.0.0 */
            InetSocketAddress(const int& port);
            /** destructor */
            virtual ~InetSocketAddress();
        
            /** returns the InetAddress */
            const InetAddress& getAddress() const{ return _inet_addr; }
            /** returns the host name for the InetAddress */
            const String& getHostName() const{ return _inet_addr.getHostName(); }
            /** returns the port number */
            const int& getPort() const{ return _port; }
            /** return the inetaddress as a string */
            String asString() const;
            
            /** returns true if valid address */
            bool isValid() const{ return _inet_addr.isValid(); }
        protected:
    		/** the inetaddress */
    		InetAddress _inet_addr;
    		/** the port number */
    		int _port;
    		/** the lowlevel data structure for this class to be used by the socket api */
            sockaddr_in _addr;
            /** called to set the low level data structure */
            void _initialize(const InetAddress &addr, const int& port);
        };
    }
}
#endif // __SRL_SocketAddress__



