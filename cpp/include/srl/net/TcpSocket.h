/**
 * File: TcpSocket.h
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


#ifndef __SRL_TCP_SOCKET__
#define __SRL_TCP_SOCKET__

#include "srl/net/Socket.h"
#include "srl/io/ByteBuffer.h"

namespace SRL
{
    namespace Net
    {
        /**
        * This class is basically a socket with a buffered reader.
        * IT IS NOT THREAD SAFE
        */
        class SRL_API TcpSocket : public Socket
        {
        public:
            TcpSocket();
            TcpSocket(const Socket::Descriptor& desc);

            /** Returns there is unread data in the local buffer or on the socket */
            bool isReadReady(int msec=SRL::NOW);
            /** 
            * reads until the character has been encountered and puts all read data into the buf 
            * 
            * @returns the number of bytes read
            */
            uint32 readUntil(String& out, const char& c, int msec=NOW);
            /** 
            * reads until the string has been encountered and puts all read data into the buf 
            * 
            * @returns the number of bytes read
            */
            uint32 readUntil(String& out, const String& str, int msec=NOW);
            /** 
            * reads until the byte has been encountered and puts all read data into the buf 
            * 
            * @returns the number of bytes read
            */
            //uint32 readUntil(SRL::byte* data, const uint32& len, const SRL::byte& c);
            /** 
            * Read until the specified number of bytes have been read or the stream as been closed
            * 
            * @returns the number of bytes read
            */
            uint32 readAll(String& out, const uint32& len);

            
            /** receive data from the socket. returns  the number of bytes received */
            int read(SRL::byte* data, int len,  int msec=FOREVER);          
            /** receive data as const char* */
            int read(const char* data, int len, int msec=FOREVER){ return recv((byte*)data,len,msec);};
            /** receive len number of bytes into buf, returns the amount read */
            int read(String &buf, int len, int msec=FOREVER);
        
        protected:
            /** read the next 1024 bytes into the bytebuffer */
            int _bufferedRecv();
        protected:
            IO::ByteBuffer _buffer;
            String _tmp;
            /** whats the max size our buffer should be */
            uint32 _max_size;
        };
    }   
}

#endif //__SRL_TCP_SOCKET__