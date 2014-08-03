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

#ifndef __SRL_STDIO__
#define __SRL_STDIO__

#include "srl/io/TextWriter.h"
#include "srl/io/TextReader.h"

namespace SRL
{
    namespace System
    {
        /** pointer to the standard output writer */
        SRL_API srl::io::TextWriter *stdout;
        /** pointer to the standard input writer */
        SRL_API srl::io::TextWriter *stderr;
        /** pointer to the standard input reader */
        SRL_API srl::io::TextReader *stdin;
    }
}
#endif

