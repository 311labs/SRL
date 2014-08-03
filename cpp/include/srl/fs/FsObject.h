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
#ifndef __SRL_FsObject__
#define __SRL_FsObject__

#include "srl/fs/FS.h"
#include "srl/DateTime.h"

namespace SRL
{
    namespace FS
    {
        /** Class represent a generic file system object */
		class SRL_API FsObject
		{
		public:
			/** constructor does not actually open the file */
			FsObject(const String &path, bool is_newed=FALSE);
        	/** default destructor */
        	virtual ~FsObject();
        	/** close the fs object (deletes it if newed) */
        	virtual void free();

            friend bool operator==(const FsObject& s1,const FsObject& s2){ return (s1._full_path == s2._full_path); };

        	/** returns the name of this object */
        	const String &name() const;
        	/** returns the path for this object */
        	const String &path() const;
        	/** return the full path and name */
        	const String &fullName() const;

        	/** returns my current permissions to the object */
        	FS::AccessModes myAccess() const;
        	/** check if the file is readable */
        	bool isReadable() const;
        	/** check if the file is writable */
        	bool isWritable() const;
        	/** check if the file is writable */
        	bool isExecutable() const;

        	/** returns the current permissions */
        	const FS::Permissions& permissions() const;
        	/** change the current permissions */
        	void changePermissions(const FS::Permissions &permissions);

        	/** returns the date & time when the filesystem object was created */
        	const DateTime &createdDate() const;
        	/** returns the date & time when the filesystem object was modified */
        	const DateTime &modifiedDate() const;
        	/** returns the date & time when the filesystem object was accessed */
        	const DateTime &accessedDate() const;	

        	/** get the current owner uid */
        	const uint32& uid() const; 
        	/** get the current group uid */
        	const uint32& gid() const;

        	/** returns true if object is a directory */
        	bool isDir();
            /** returns true if the object is a file */
            bool isFile();
            /** returns true if the object is a symlink */
            bool isSymLink();
            
        	/** returns the size in bytes */
        	const uint64& size() const;
        
        // BEGIN Change Event Listener Functions (Monitor this object for changes)
        	/** start listening for file size change events */
        	void startChangeListener();
        	/** stop listening for file size change events */
        	void stopChangeListener();
        	
        	/** returns with the filesystem object reports a change
        	* Currently this only looks at file size changes
        	* TODO add different types of change monitoring as parameters
        	*/
        	virtual void waitForChange();
        	/** change flag, this needs to be public for the signal handler */
        	bool _is_changed;
        // END Change Event Listener
        
        protected:
			/** default constructor */
			FsObject(){};
			/** if object is newed */
			bool _is_newed;
        #ifdef WIN32
			/** handle for change monitor */
			HANDLE _change_handle;
        #else
            int _kq, _fd;
        #endif
			/** update the date times */
			void _update_stats() const;
		private:
			/** full name and path */
			String _full_path, _name, _path;
			/** File Date Times */
			mutable DateTime _dt_created, _dt_modified, _dt_accessed;
			/** permission structure */
			mutable FS::Permissions _permissions;
			mutable uint32 _st_mode;
			/** stores the size of the file in bytes */
			mutable uint64 _file_size;
			/** stores the uid and gid */
			mutable uint32 _file_uid, _file_gid;
			/** have we updated the date times */
			mutable Bool _did_stats;
		};
        
    }
}

#endif 

