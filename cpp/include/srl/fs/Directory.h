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
#ifndef __SRL_DIRECTORY__
#define __SRL_DIRECTORY__

#include "srl/fs/FsObject.h"
#include "srl/util/Vector.h"

// FIXME possible memory problems 
// one thing is to fix linux incace diropen is null

namespace SRL
{
    namespace FS
    {
		/** generic file base class (NOT THREAD SAFE) */
		class SRL_API Directory : public FsObject
		{
		public:
			/** basic Directory constructor */
			Directory(const String &path, bool newed=false);
			/** default destructor */
			~Directory();
        	/** close the fs object (deletes it if newed) */
        	virtual void free();
			/** returns the parent directory */
			Directory* getParent() const;
			/** refresh the current state */
			void refresh();
			/** return the number of children */
			uint32 totalChildren() const {return _children.size(); }
			/** return the child by index */
			FsObject* getChild(uint32 index) const { return _children[index]; }
			/** check if we have the child by name */
			bool hasChild(const String& name);
			
		protected:
			bool _is_initialized;
			Util::Vector<FsObject*> _children;
		};        
    }   
}

#endif // __SRL_DIRECTORY__

