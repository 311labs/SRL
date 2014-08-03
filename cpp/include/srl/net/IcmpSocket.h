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

#ifndef __SRL_IcmpSocket__
#define __SRL_IcmpSocket__

#include "srl/net/Socket.h"
#include "srl/sys/Thread.h"
#include "srl/net/IcmpMessage.h"

namespace SRL
{
    namespace Net
    {
        namespace ICMP
        {
            class Message;
            class RawMessage;
            class Response;
            
            /**
            * The Internet Control Message Protocol (ICMP) is one of the core protocols of the
            * Internet protocol suite. It is chiefly used by networked computers' operating
            * systems to send error messages indicating, for instance, that a requested
            * service is not available or that a host or router could not be reached.
            * ICMP differs in purpose from TCP and UDP in that it is usually not used directly
            * by user network applications. One exception is the ping tool, which sends ICMP
            * Echo Request messages (and receives Echo Response messages) to determine whether
            * a host is reachable and how long packets take to get to and from that host.
            *
            * TODO need some major work on how to handle receiving packets other then echo replies
            */
            class SRL_API IcmpSocket : protected System::Thread
            {
            public:
                /** Interface that is used for callbacks on incoming icmp packets */
                class Listener
                {
                public:
                    virtual ~Listener(){}
                    virtual void icmp_echo_reply(ICMP::Message &reply)=0;
                    virtual void icmp_unknown(ICMP::Message &msg)=0;
                };
                /** create a ICMP socket
                * @param id the id that is included in the header of all icmp packets sent from this computer
                */
                IcmpSocket(uint16 id=0);
                /** default destructor */
                virtual ~IcmpSocket();
    
                /** starts the ICMP Listener */
                void startListener(IcmpSocket::Listener *listener);
                /** stops the ICMP Listener */
                void stopListener();
                
                /** send a IcmpPacket to */
                int sendTo(const InetSocketAddress &addr, const ICMP::RawMessage &msg) const;
                /** receive an IcmpPacket */
                int recvFrom(ICMP::RawMessage &msg, int timeout);
                
                /** send echo request */
                void sendEchoRequest(const InetSocketAddress &addr, const uint32 &sequence, SRL::uint16 msg_size=56, SRL::uint16 ttl=0);
                /** receive a echo reply (wait for the timeout period in milliseconds)
                * @param timeout maximum time in milliseconds to wait for an incoming message
                */
                ICMP::Response receiveEchoReply(int timeout);
                
                /** returns the socket for this server socket */
                Socket& getSocket(){ return _socket; }
                
            protected:
                /** called when thread is started */
                virtual bool initial();
                /**called when thread is finished */
                virtual void final();
                /** thread run */
                virtual bool run();
            
                /** socket to send pings out on */
                Socket _socket;
                /** Object Used for Sending Pings */
                RawMessage* _echo_request;
                /** Object Used for Receiving Pings */
                RawMessage* _echo_reply;
                /** listener used to send icmp packets to */
                IcmpSocket::Listener *_listener;
                /** packet uid */
                SRL::uint16 _uid;
                
            };
        }
    }
}

#endif //__SRL_IcmpSocket__


