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
#ifndef __SRL__Exceptions__
#define __SRL__Exceptions__

#include "srl/String.h"

namespace SRL
{
    namespace Errors
    {
    	/** Base class for all srl exceptions */
    	class SRL_API Exception
    	{
    	public:
    		/**
    		* default constructor
    		* @param message the message describing the exception
    		*/
    		Exception(String message): _message(message){};
    		/** default destrructor */
    		virtual ~Exception(){};

    		/** returns the message describing the exception */
    		const String& message() const{ return _message;}
    	protected:
    		/** variable holding the exception message */
    		String _message;
    	};

    	/** Thrown for generic logic errors */
    	class SRL_API LogicException : public Exception
    	{
    	public:
    		/** default constructor */
    		LogicException(String message): Exception(message){};
    	};

    	/** Thrown for generic IO errors */
    	class SRL_API IOException : public Exception
    	{
    	public:
    		/** default constructor */
    		IOException(String message): Exception(message){};
    	};

    	/** Thrown when a null pointer is handled */
    	class SRL_API NullPointerException : public Exception
    	{
    	public:
    		/** default constructor */
    		NullPointerException(String message): Exception(message){};
    	};

    	/** Thrown when alloc or new call fails */
    	class SRL_API OutOfMemoryException : public Exception
    	{
    	public:
    		/** default constructor */
    		OutOfMemoryException(String message): Exception(message){};
    	};

    	/** Thrown when a container (array, list, collection) is out of bounds */
    	class SRL_API IndexOutOfBoundsException : public Exception
    	{
    	public:
    		/** default constructor */
    		IndexOutOfBoundsException(String message): Exception(message){};
    	};

    	/** Thrown when when access was denied by the operating system */
    	class SRL_API PermissionDeniedException : public IOException
    	{
    	public:
    		/** default constructor */
    		PermissionDeniedException(String message): IOException(message){};
    	};

    	/** Thrown when the argument passed into a method is invalid */
    	class SRL_API InvalidArgumentException : public LogicException
    	{
    	public:
    		/** default constructor */
    		InvalidArgumentException(String message): LogicException(message){};
    	};

    	/** Thrown when a method is not yet implemented */
    	class SRL_API NotImplementedException : public LogicException
    	{
    	public:
    		/** default constructor */
    		NotImplementedException(String message): LogicException(message){};
    	};

    	/** Thrown when an unknown error has occured */
    	class SRL_API UnknownException : public Exception
    	{
    	public:
    		/** default constructor */
    		UnknownException(String message): Exception(message){};
    	};
    	
    	
    	
    }
}

#endif // __SRL__Exceptions__
