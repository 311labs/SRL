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

#ifndef __SRL_FS__
#define __SRL_FS__

#include "srl/sys/System.h"
#include "srl/Exceptions.h"

namespace SRL
{
	namespace FS
	{
		namespace Errors
		{
	
			/** Thrown when the file object could not be found  */
			class SRL_API FileNotFoundException : public SRL::Errors::IOException
			{
			public:
				/** default constructor */
				FileNotFoundException(String message): SRL::Errors::IOException(message){};
			};
	
		}

		/** enumerate access */
		enum AccessModes
		{
			NONE=0,
			EXECUTE			= 0x01,
			WRITE				= 0x02,
			WRITE_EXECUTE		= 0x03,
			READ				= 0x04,
			READ_EXECUTE		= 0x05,
			READ_WRITE			= 0x06,
			READ_WRITE_EXECUTE	= 0x07
		};

		/** file system permissions structure */
		struct SRL_API Permissions
		{
		    Permissions(uint8 u=0, uint8 g=0, uint8 o=0):owners(u),groups(g),others(o){}
			uint8 owners;
			uint8 groups;
			uint8 others;
			
			String asString() const;
		};


        /** check if file or directory exists */
		extern SRL_API bool Exists(const String &path);
		/** Create a Directory */
		extern SRL_API void MakeDir(const String &path, Permissions *permissions=NULL, Bool recursive=FALSE);
		/** Create a SymLink */
		extern SRL_API void MakeSymLink(const String &from_path, const String &to_path);
		/** move file or directory */
		extern SRL_API void Move(const String &from_path, const String &to_path, Bool replace_existing=FALSE);
		/** copy file or directory */
		extern SRL_API void Copy(const String &from_path, const String &to_path, Bool replace_existing=FALSE);	
		/** delete file or directory (returns the number of items deleted) */
		extern SRL_API uint32 Delete(const String &path, Bool recursive=false);
	
	    /** returns the current path permissions */
	    extern SRL_API Permissions GetPermissions(const String& path);
		/** Permissions to unix mode */
		extern SRL_API uint32 PermissionsToModes(Permissions &permissions);
		/** Permissions to unix mode */
		extern SRL_API Permissions ModesToPermissions(uint32 &modes);
		
		/** return the uid for a file */
		extern SRL_API uint32 GetUserID(const String &path);
		/** return the gid for a file */
		extern SRL_API uint32 GetGroupID(const String &path);
		/** change owner ship of a file */
        extern SRL_API void ChangeOwner(const String &path, uint32 uid, uint32 gid);

		/** returns the user name for the given uid */
		extern SRL_API String GetUserFromUID(uint32 uid);
		/** returns the group name for the given uid */
		extern SRL_API String GetGroupFromGID(uint32 gid);

        /** returns the real path of the path (follows sym-links)*/
        extern SRL_API String GetRealPath(const String &path);

		/** returns the file name from the path */
		extern SRL_API String GetFileName(const String &path);
		/** returns the ext from the path */
		extern SRL_API String GetExt(const String &path);
		/** removes the directory name from the path */
		extern SRL_API String GetDirName(const String &path);
		/** returns the system seprator '\' or '/' */
		extern SRL_API char GetSysSeperator();
		/** returns the current working directory */
		extern SRL_API String GetCurrentDir();
		/** sets the current working directory */
		extern SRL_API bool SetCurrentDir(const String &path);
		/** tells us if the path is a directory */
		extern SRL_API bool IsFile(const String &path);
		/** tells us if the path is a directory */
		extern SRL_API bool IsSymLink(const String &path);				
		/** tells us if the path is a directory */
		extern SRL_API bool IsDir(const String &path);
		/** tells us if the path is a block device */
		extern SRL_API bool IsBlockDevice(const String &path);
		/** tells us if the path is a character device */
		extern SRL_API bool IsCharDevice(const String &path);
        /** tell us if the file is executable */
        extern SRL_API bool IsExecutable(const String &path);
	}
}


#endif // __SRL_FS__
