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
#ifndef __SRL_MulticastSocket__
#define __SRL_MulticastSocket__

#include "srl/net/DatagramSocket.h"

namespace SRL
{
	namespace Net
	{
		/**
		* Multicast is the delivery of information to a group of destinations
		* simultaneously using the most efficient strategy to deliver the
		* messages over each link of the network only once and only create
		* copies when the links to the destinations split. (wikipedia)
		*/
		class SRL_API MulticastSocket : public DatagramSocket
		{
		public:
			/** create a multicast socket unbound */
			MulticastSocket();
			/** create a multicast socket bound to the specified port */
			MulticastSocket(const int& port);
			/** create a multicast socket bound to the specified address */
			MulticastSocket(const InetSocketAddress &addr);
			/** default destructor */
			virtual ~MulticastSocket();
			/** @short Join a multicast group 
			*
			* Every IP multicast group has a group address. IP multicast provides
			* only open groups: That is, it is not necessary to be a member of a group
			* in order to send datagrams to the group. 
			*
			* Multicast address are like IP addresses used for single hosts, and is written
			* in the same way: A.B.C.D. Multicast addresses will never clash with host
			* addresses because a portion of the IP address space is specifically reserved
			* for multicast. This reserved range consists of addresses from 224.0.0.0 to
			* 239.255.255.255. However, the multicast addresses from 224.0.0.0 to
			* 224.0.0.255 are reserved for multicast routing information; Application programs
			* should use multicast addresses outside this range.
			*/
			void joinGroup(const InetAddress &mcastaddr);
			/** Leave a multicast group */
			void leaveGroup(const InetAddress &mcastaddr);
			/** @short Set the interface to send multicast data from
			*
			* If the host has more than one interface and the IP_MULTICAST_IF option
			* is not set, multicast transmissions are sent from the default interface
			*/
			void setMulticastInterface(const InetAddress &inf);
			/** Retreive the current MultiCast Interface */
			InetAddress getMulticastInterface();
			/** @short Set the Multicast Packet Loop Back option
			*
			* control the loopback of multicast packets. By default, packets are delivered
			* to all members of the multicast group including the sender, if it is a member
			* disable loopback mode if you don't want to receive packets back
			*/
			void setLoopbackMode(bool enable);
			/** returns the current loopback mode */
			bool getLoopbackMode() const;
			/** Retrieve the Multicast Time to Live option */
			uint8 getTimeToLive() const;
			/** @short Sets the Multicast Time to Live option
			*
			* The IP multicast routing protocol uses the Time To Live (TTL) field of
			* IP datagrams to decide how "far" from a sending host a given multicast
			* packet should be forwarded. The default TTL for multicast datagrams is
			* 1, which will result in multicast packets going only to other hosts on the
			*  local network.	 
			*/
			void setTimeToLive(uint8 ttl);
		};
	}
}

#endif //__SRL_MulticastSocket__

