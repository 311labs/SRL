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
#ifndef __SRL_ICMP__
#define __SRL_ICMP__

#include "srl/types.h"
#include "srl/exports.h"
#include "srl/system.h"

// namespace SRL
// {
//  namespace Net
//  {
//      /** standard ipv4 header */
//      struct IpHeader
//      {
//          SRL::byte ip_vhl;         /* Version and Header Length */
//          SRL::byte ip_tos;         /* Type of Service byte */
//          SRL::uint16 ip_len;         /* Total length of datagram */
//          SRL::uint16 ip_id;          /* Identification of datagram */
//          SRL::uint16 ip_off;         /* Fragmentation offset */
//          SRL::byte ip_ttl;         /* Time to live byte */
//          SRL::byte ip_p;           /* Protocol of user level datagram */
//          SRL::uint16 ip_sum;         /* Header checksum */
//          SRL::uint32 ip_src;         /* Source address of datagram */
//          SRL::uint32 ip_dst;         /* Destination address of datagram */
//      };
//      const int32 IP_HEADER_SIZE = 20;
//      
//      namespace ICMP
//      {
//          /** 8 byte icmp header */
//          struct Header
//          {
//              SRL::byte type;
//              SRL::byte code;
//              SRL::uint16 checksum;
//              SRL::uint16 id;
//              SRL::uint16 seq;
//          };
//      
//          /** our echo request packet */
//          struct EchoRequest
//          {
//              Header icmp;
//              SRL::float64 timestamp;
//              SRL::byte data[1];
//          } ;
//      
//          /** our echo reply packet */
//          struct EchoReply
//          {
//              IpHeader ip;
//              Header icmp;
//              SRL::float64 timestamp;
//              SRL::byte data[1];
//          };
//  
//          /** destination unreachable */
//          struct DestinationUnreachable
//          {
//              IpHeader ip;
//              SRL::byte type;
//              SRL::byte code;
//              SRL::uint16 checksum;
//              struct Original
//              {
//                  IpHeader ip;
//                  Header icmp;
//              } original;
//          };
//      
//          const int32 HEADER_SIZE = 8;
//          const int32 ECHO_REQ_SIZE = 16;
//          const int32 ECHO_REP_SIZE = 36;
//      }
//  }
// }

#endif //__SRL_ICMP__

