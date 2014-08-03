/*********************************************************************************
* Copyright (C) 1999,2004 by srl, Inc.   All Rights Reserved.        *
*********************************************************************************
* Authors: Ian C. Starnes, Dr. J. Shane Frasier, Shaun Grant
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
*******************************************************************************                                                     
* Last $Author: ians $     $Revision: 216 $                                
* Last Modified $Date: 2005-12-28 19:52:10 -0500 (Wed, 28 Dec 2005) $                                  
******************************************************************************/

#ifndef __SRL_NetworkInterface__
#define __SRL_NetworkInterface__

#include "srl/String.h"
#include "srl/net/InetAddress.h"
#include "srl/util/Vector.h"

/*TODO gracefully delete network interface instances or set them as inactive
        and remove them from the active interface list
   TODO deal with updating new vs old.. need to update ip address essid etc
*/
namespace SRL
{
    namespace Net
    {
        class NetworkInterface;
        typedef Util::Vector<NetworkInterface*> NetworkInterfaces;
        /** Network Interface (eth0, eth1, lo) */
        class SRL_API NetworkInterface
        {
        public:
            enum ADAPTOR_TYPE
            {
                WIRED,
                LOOPBACK,
                WIRELESS,
                POINTTOPOINT,
                UNKNOWN
            };

            class SRL_API IO
            {
            friend class NetworkInterface;
            public:
                /** total bytes */
                const uint64& bytes() const{ return _bytes; }
                /** total packets */
                const uint64& packets() const{ return _packets; }
                /** total multicast */
                const uint64& multicast() const{ return _multicast; }
                /** total errors */
                const uint64& errors() const{ return _errors; }
                /** total dropped */
                const uint64& dropped() const{ return _drops; }
            protected:
                IO() : _bytes(0), _packets(0), _multicast(0), _errors(0), _drops(0){}
            
            private:
                uint64 _bytes, _packets, _multicast, _errors, _drops;
            };
            /** return the name */
            const String& name() const{ return _name;}
            /** return the current ip */
            const String& ip() const{ return _ip; }
            /** returns the current mac address */
            const String& mac() const{ return _mac; }
            /** returns the broadcast address */
            const String& broadcast() const{return _broadcast;}
            /** return the netmask */
            const String& netmask() const{ return _netmask;}
            /** (deprecated) return the netmask */
            const String& gateway() const{ return _gateway;}
            /** received info */
            const IO& rx() const{ return _rx; }
            /** transmitted info */
            const IO& tx() const{ return _tx; }
            /** received info */
            const IO& drx() const{ return _drx; }
            /** transmitted info */
            const IO& dtx() const{ return _dtx; }
            
            /** number of collisions */
            const uint32& collisions() const{ return _collisions; }
            /** maximum transmission unit size  */
            const uint32& mtu() const{ return _mtu; }
            /** routing metric */
            const uint32& metric() const{ return _metric; }
            /** current max speed the interface supports */
            const uint32& speed() const{ return _speed; }
            
            /** return true if connection is up */
            bool isUp() const { return _is_up; }
            /** has link (plugged in) */
            bool hasLink() const{ return _has_link;}
            
            /** the adaptor type (wired, loopback, wireless) */
            ADAPTOR_TYPE type(){ return _type; }
            
            /** return true if wireless connection */
            bool isWifi() const { return (_type == WIRELESS); }
            /** returns true if this is the loopback device */
            bool isLoopback() const{ return (_type == LOOPBACK);}           
            
            /** return the essid of the wireless connection */
            const String& essid() const { return _essid; }
            /** return the AP MAC address */
            const String& apMac() const { return _ap_mac; }
            
            /** update rx/tx info (returns false if interface is no longer active) */
            bool update();
            
            /** set the ipv4 address for this interface */
            void setIP(const String& ip);
            /** set the ipv4 address for this interface */
            void setNetmask(const String& ip);
            /** set the ipv4 broadcast address for this interface */
            void setBroadcast(const String& ip);
            
            virtual ~NetworkInterface();
        protected:
            NetworkInterface(String name);
            /** name and ip */
            String _name, _mac;
            /** routing information */
            String _ip, _netmask, _broadcast, _gateway;
            /** maximum transmission unit, speed */
            uint32 _mtu, _metric, _speed;
            /** connection status (has_link, connecting, connected) */
            bool _has_link, _is_up;
            /** adaptor type */
            ADAPTOR_TYPE _type;
            /** extra information for wireless adaptors */
            String _essid, _ap_mac;
            /** stats */
            IO _rx, _tx, _drx, _dtx;
            /** collisions */
            uint32 _collisions;
            /** used internally as the index */
            uint32 _device_type;
    
        /** STATIC METHODS */
            static bool __update();
            static void _update_interface(NetworkInterface* iface, bool is_new);
            static void _remove_dead(NetworkInterfaces *list);
            static void _delete_dead();
            static NetworkInterfaces* __interfaces;
            static NetworkInterfaces* __newifs;
            static NetworkInterfaces* __deadifs;
        public:
            /** Return All known network interfaces as vector */
            static NetworkInterfaces* GetAll(bool do_update=true);
            /** Return a network interface by name (not found returns null) */
            static NetworkInterface* GetByName(const String& name, bool do_update = true);
            /** Return network interface by IP/Hostname */
            static NetworkInterface* GetByAddress(const String& address, bool do_update = true);
            /** Return network interface by IP/Hostname */
            static NetworkInterface* GetByAddress(InetAddress address, bool do_update=true);
            
            /** returns the recently discovered interfaces */
            static NetworkInterfaces* GetNew(){ return __newifs; }
            /** returns the recently destroyed interfaces */
            static NetworkInterfaces* GetDead(){ return __deadifs; }
            /** keeps track of the last update time */
            static float64 __last_update;
        };
    }
}
#endif

