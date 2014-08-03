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

#ifndef __SRL_LIST__
#define __SRL_LIST__

#include "srl/util/Map.h"

namespace SRL
{
    namespace Util
    {
        /**
        *   List Interface
        *   This is the base class to all list type containers 
        */        
        template<class Value, class Iterator>
        class List
        {
        public:
	        /** add an object to the list */
	        virtual void add(const Value& value)=0;
	        /** add an object to the back of the list */
	        virtual void addAt(const uint32& index, const Value& value)=0;       
	        /** remove object from list by value */
	        virtual void remove(const Value& value)=0;
	        /** remove object from list by index  */
	        virtual void removeAt(const uint32 &index)=0;

	        /** remove all objects */
	        virtual void clear(bool do_delete=false)=0;
	        
	        /** replace the current item at index with the new one returning the old one */
	        virtual Value& set(const uint32& index, const Value& value)=0;
	        
	        /** retrieve the object by index */
	        virtual Value& get(const uint32& index)const=0;
			/** return the value object based on a key */
			virtual Value& operator[](const uint32& index)const=0;

	        /** does this map contain an entry with this value */
	        virtual bool containsValue(const Value& value)const=0;
	        
	        /** position of value */
	        virtual int positionOf(const Value& value, int start_index=0)const=0;
	        
	        /** sort the list */
	        virtual void sort(bool descending=true)=0;
	        
	        /** returns the size of the map */
	        virtual const uint32& size() const =0;
	        /** returns true if the map is empty */
	        bool isEmpty() const{ return (size() == 0);}
	        

			
			/** returns the first iterator in the map */
			virtual Iterator begin() const =0;
			/** return the end of iteration iterator, like eof */
			virtual const Iterator& end() const =0;
        };
    }
}

#endif // __SRL_LIST__

