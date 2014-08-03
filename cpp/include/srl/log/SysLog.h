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
#ifndef __SRL_SystemLog__
#define __SRL_SystemLog__

#include "srl/log/Logger.h"

namespace SRL
{
	namespace Log
	{
		/**
		 *  Creates a Log that uses the Operating System logging facilities 
		 * @author Ian Starnes
		 * @short Uses the current system logging facilities like syslog 
		 */
		class SRL_API SysLog : public LoggerOutput
		{
			public:
				/** Constructor */
				SysLog(const String& name, const Logger::Priority& loglevel=Logger::INFO);
				/** Add output to type to Logger */
				static void Create(const String& name,
							const Logger::Priority& loglevel=Logger::INFO);
				/** main log method */
				virtual void log(const Logger::Priority& priority, 
						const String& name,  const String& message);
	
			protected:
				/** Destructor */
				virtual ~SysLog() throw();
				String _syslog_name;
				System::Mutex _syslog_mutex;
				
		};	
	}
}

#endif
