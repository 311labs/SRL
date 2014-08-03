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
#ifndef __SRL_DatagramSocket__
#define __SRL_DatagramSocket__

#include "srl/net/Socket.h"

namespace SRL
{
	namespace Net
	{
		/**
		* The User Datagram Protocol (UDP) is one of the core protocols of the Internet
		* protocol suite. Using UDP, programs on networked computers can send short
		* messages known as datagrams to one another. UDP does not provide the reliability
		* and ordering guarantees that TCP does; datagrams may arrive out of order or go
		* missing without notice. However, as a result, UDP is faster and more efficient
		* for many lightweight or time-sensitive purposes. Also its stateless nature is
		* useful for servers that answer small queries for huge numbers of clients.
		*/
		class SRL_API DatagramSocket
		{
		public:
			/** create a datagram socket bound to any port and host */
			DatagramSocket();
			/** create a datagram socket bound to the local port */
			DatagramSocket(const int& port);
			/** create a datagram socket bound to the local port */
			DatagramSocket(const InetSocketAddress &endpoint);
			/** default destructor */
			virtual ~DatagramSocket();

			/** send string as datagram payload */
			int sendTo(const InetSocketAddress &to_addr, const SRL::String &data, uint32 flags=0);
			/** send a datagram(UDP Packet) */
			int sendTo(const InetSocketAddress &to_addr, const SRL::byte *data, uint32 len, uint32 flags=0);
			/** receive a datagram */
			int recvFrom(InetSocketAddress &from_addr, SRL::byte *buffer, uint32 len, uint32 flags=0);

    		/** Returns the current Recv state of the socket */
    		bool isRecvReady(int msec=SRL::NOW){ return _socket.isRecvReady(msec); }

			/** close the socket */
			void close(){ _socket.close(); }
			
			/** returns the socket for this datagram socket */
			Socket& getSocket(){ return _socket; }
			
			/** returns the local address the socket is bound on */
			const InetSocketAddress& getLocalAddress()const{ return _socket.getLocalAddress(); }
			/** Retrieve the Multicast Time to Live option */
            uint8 getTimeToLive() const{ return _socket.getTimeToLive(); };
			/** @short Sets the Multicast Time to Live option
			*
			* The IP multicast routing protocol uses the Time To Live (TTL) field of
			* IP datagrams to decide how "far" from a sending host a given multicast
			* packet should be forwarded. The default TTL for multicast datagrams is
			* 1, which will result in multicast packets going only to other hosts on the
			*  local network.	 
			*/
            void setTimeToLive(uint8 ttl){ _socket.setTimeToLive(ttl);};			
		protected:
			Socket _socket;				
		};
	}
}

#endif //__SRL_DatagramSocket__

