/*
* (c) Copyright 2009 Ian Starnes. All Rights Reserved. 
* 
* Registry.h is part of 
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
* @date August 02 2009  
*/
#include <windows.h>
#include "srl/String.h"

namespace SRL
{
    namespace Windows
    {
        namespace Registry
        {
			extern SRL_API bool HasKey(HKEY root_key, const String& key);
			extern SRL_API bool GetValue(HKEY root_key, const String& key, const String& name, String& value);
			extern SRL_API bool GetValue(HKEY root_key, const String& key, const String& name, int32& value);
			extern SRL_API bool CreateKey(HKEY root_key, const String& key);
			extern SRL_API bool SetValue(HKEY root_key, const String& key, const String& name, const String& value);
			extern SRL_API bool SetValue(HKEY root_key, const String& key, const String& name, const int32* value);
        } /* Registry */ 
    } /* Windows */ 
} /* SRL */ 
