/**
 * File: ServerSocket.h
 * Project: srl3
 * Created by: Ian Starnes on 2006-11-21.
 * 
 * This library is free software; you can redistribute it and/or                 
 * modify it under the terms of the GNU Lesser General Public                    
 * License as published by the Free Software Foundation; either                  
 * version 2.1 of the License, or (at your option) any later version.            
 *                                                                               
 * This library is distributed in the hope that it will be useful,               
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             
 * Lesser General Public License for more details.                               
 *                                                                               
 * You should have received a copy of the GNU Lesser General Public              
 * License along with this library; if not, write to the Free Software           
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.   
 */

#ifndef _SERVERSOCKET_H_
#define _SERVERSOCKET_H_

#include "srl/net/Socket.h"
#include "srl/sys/Thread.h"
#include "srl/util/Vector.h"

namespace SRL
{
    namespace Net
    {
        
        /**
        * Server Socket
        * TODO keep list of descriptors (notify of new data in buffer)
        * TODO logic to select on all connections
        * 
        */
        class SRL_API ServerSocket : protected System::Thread
        {
        public:
            /** Server Socket Event Listener  */
            class EventListener
            {
            public:
                EventListener(){}
                virtual ~EventListener(){}
                virtual void connection_new(Socket::Descriptor &desc)=0;
                virtual void connection_recvReady(Socket::Descriptor &desc)=0;
                virtual void connection_closed(Socket::Descriptor &desc)=0;
            };
    		/** creates a server socket bound to the specified port */
    		ServerSocket(const int& port);
    		/** creates a server socket bound to the specified address */
    		ServerSocket(const InetSocketAddress& endpoint);
    		/** destructor */
    		virtual ~ServerSocket();

            /** Starts the event listener thread
            * This will cause the socket to listen for new connections
            * and call the connection_new event on each new connection
            */
            void startListener(EventListener *listener);
            /** Stops the event listener thread
            * this will close all open connections
            */
            void stopListener();
            /** polls all connections for incoming data and calls the listener
            * connection_recvReady */
            void waitForRecv(const int32& msec=SRL::FOREVER);

    		/** Listen for connections on a socket */
    		void listen(int maxcons=SOMAXCONN){ _socket.listen(maxcons); }
    		/** Accept a connection on the socket */
    		Socket::Descriptor accept();

    		/** returns the local address */
    		const InetSocketAddress &getLocalAddress() const{ return _socket.getLocalAddress(); }

    		/** returns the socket for this server socket */
    		Socket& getSocket(){ return _socket; }

    		/** close this socket */
    		void close(){ _socket.close(); }
    	protected:
    		Socket _socket;
    		/** list of connections */
    		Util::Vector<Socket::Descriptor> _connections;
    		/** listener */
    		EventListener *_listener;
    		/** event listener thread functions */
    		bool initial();
    		bool run();
    		void final();
        };
    }
}


#endif /* _SERVERSOCKET_H_ */

