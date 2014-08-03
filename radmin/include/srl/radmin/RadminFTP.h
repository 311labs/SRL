/*
* (c) Copyright 2009 Ian Starnes. All Rights Reserved. 
* 
* RadminFTP.h is part of 
*  is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  
* @author Ian Starnes <ian@starnes.de>
* @date August 31 2009  
*/
#include "srl/net/ServerSocket.h"
#include "srl/net/TcpSocket.h"
#include "srl/fs/BinaryFile.h"

namespace SRL
{
    namespace Radmin 
    {
        /**
        * RadminFTP
        * Transfer File logic for Radmin
        */
        class RadminFTP : public SRL::Net::ServerSocket::EventListener
        {
        public:
            /** default contstructor */
            RadminFTP(int port, const String& path, bool get_file=true);
            /** default contstructor */
            RadminFTP(int port, const String& path, const String& username, bool get_file=true);            
            /** default destructor */
            virtual ~RadminFTP();

			virtual void connection_new(Net::Socket::Descriptor &desc);
			virtual void connection_recvReady(Net::Socket::Descriptor &desc);
			virtual void connection_closed(Net::Socket::Descriptor &desc);
            
            const bool& isDone() const { return _is_done; }

        protected:
            bool _is_getting, _is_done;
            SRL::Net::ServerSocket* _server;
            SRL::Net::TcpSocket* _connection;
            SRL::FS::BinaryFile* _file;
            String _path, _username;
            SRL::byte _buffer[256];
            
        };
        
    } /* Radmin */
} /* SRL */