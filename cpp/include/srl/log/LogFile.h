/*********************************************************************************
* Copyright (C) 1999,2004 by Ian C Starnes.   All Rights Reserved.        *
*********************************************************************************
* Sub Authors: J. Shane Frasier, Shaun Grant, Saqib Ali, Mark Feldhousen
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
* Last $Author: ians $     $Revision: 258 $                                
* Last Modified $Date: 2006-04-10 18:45:20 -0400 (Mon, 10 Apr 2006) $                                  
******************************************************************************/

#ifndef __SRL_LogFile__
#define __SRL_LogFile__


#include "srl/fs/TextFile.h"
#include "srl/log/Logger.h"
#include "srl/DateTime.h"

namespace SRL
{
	
	namespace Log
	{
	
		/**
		* @author Ian Starnes
		* $Author: ians $
		* $Revision: 258 $
		* $Date: 2006-04-10 18:45:20 -0400 (Mon, 10 Apr 2006) $
		* @short Blah
		*/
		class SRL_API LogFile : public LoggerOutput, protected FS::TextFile
		{
		public:
			/** Constructor */
			LogFile(const String& filename, const Logger::Priority& loglevel=Logger::INFO);
			/** Add output to type to Logger */
			static void Create(const String& filename,
						const Logger::Priority& loglevel=Logger::INFO);
			/** main log method */
			virtual void log(const Logger::Priority& priority, const String& name, 
				const String& message);
		
		protected:
			/** Destructor */
			virtual ~LogFile() throw();
			
			/** Holds the date time */
			DateTime _sysTime;
		};
		
	} // namespace Log

} // namespace SRL


#endif // __SRL_LogFile__
