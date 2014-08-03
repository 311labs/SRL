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
* Last $Author: ians $     $Revision: 240 $                                
* Last Modified $Date: 2006-03-03 04:25:36 -0500 (Fri, 03 Mar 2006) $                                  
******************************************************************************/

#ifndef __SRL_Pipe__
#define __SRL_Pipe__

#include "srl/FsNode.h"
#include "srl/net/Socket.h"
#include "srl/Serializer.h"
#include "srl/ByteBuffer.h"

namespace SRL
{
	namespace FS
	{
	    /** A Class used for interprocess communication using Unix-domain sockets (UNIX) or 
         * named pipe/mailslot mechanisms (WIN32). */
	    class Pipe
	    {
	    public:
	        enum Type {MESSAGE, STREAM};
	        
	        Pipe(Type pipe_type=STREAM);
	        virtual ~Pipe();
	        
	        /** connect to the specified pipe */
	        void connect(const String& name);
	        /** create a pipe to listen on */
	        void listen(const String &name);
	        /** accept incoming connections on the pipe */
	        void accept(Pipe *pipe=NULL);
	        /** close the pipe */
	        void close();

            /** tests if the pipe is open or not */
            bool isOpen() const;

    		/** Returns the current Send state of the socket */
    		bool isSendReady(int msec=SRL::NOW);
    		/** Returns the current Recv state of the socket */
    		bool isRecvReady(int msec=SRL::NOW);

    		/** sends data accross the pipe. returns the number of bytes sent */
    		int send(const SRL::byte* data, int len, int msec = SRL::FOREVER);
    		/** sends data as const char */
    		int send(const char* data, int len, int msec = SRL::FOREVER);
     		/** Send the following string */
    		int send(String msg, int msec=SRL::FOREVER);
    		/** send the byte buffer */
    		int send(SRL::Collections::ByteBuffer &buf, int msec=SRL::FOREVER);
		   		
    		/** receive data from the pipe. returns  the number of bytes received */
    		int recv(SRL::byte* data, const int& len,  int msec = SRL::FOREVER);
    		/** receive len number of bytes into buf, returns the amount read */
    		int recv(String &buf, int len, int msec=SRL::FOREVER);
    		/** receive into a byte buffer */
    		int recv(SRL::Collections::ByteBuffer &buf, int msec=SRL::FOREVER);	    
	    protected:
	        /** opens the pipe */
	        void open(const String& name);
	    
	    private:
	    #ifdef WIN32
	        HANDLE _handle;
	    #else
	        SOCKET _handle;
	    #endif
	        /** type of pipe this is (MESSAGE, STREAM) */
	        Type _type;
	        /** name of the pipe */
	        String _name;
	        /** location of the pipe (fullpath + name) */
	        String _location;
	        /** flag for connection state */
	        bool _connected;
	    };
	}
}


#endif // __SRL_Pipe__