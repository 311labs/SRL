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

#ifndef __SRL_MAP__
#define __SRL_MAP__

#include "srl/util/Map.h"

namespace SRL
{
    namespace Util
    {
        template<class Key, class Value>
        class Map
        {
        public:
	        /** add an object to the map */
	        virtual void add(const Key& key, const Value& value)=0;
	        /** remove object from map */
	        virtual void remove(const Key& key)=0;
	        
	        /** retrieve the object by key */
	        virtual Value& get(const Key& key)=0;
			/** return the value object based on a key */
			virtual Value& operator[](const Key& key)=0;
				        
	        /** does this map contain an entry with the key */
	        virtual bool containsKey(const Key& key)=0;
	        /** does this map contain an entry with this value */
	        virtual bool containsValue(const Value& value)=0;
	        
	        /** returns the size of the map */
	        virtual int size() const =0;
	        /** returns true if the map is empty */
	        bool isEmpty() const{ return (size() == 0);}
	        
	        /** remove all objects */
	        virtual void clear(bool do_delete=false)=0;
	        
	        // NOW OUR INTERATOR INTERFACE
			class Iterator
			{
			public:
				/** assignment operator */
				virtual Iterator& operator=(const Iterator& iter)=0;
				/** (++iter)moves the iterator forward by one  */
				virtual void operator++()=0;
				/** (iter++)moves the iterator forward by one  */
				virtual Iterator operator++(int n)=0;
				/** (--iter)mover the iterator backwards by one */
				virtual void operator--()=0;
				/** (iter--)mover the iterator backwards by one */
				virtual Iterator operator--(int n)=0;
				/** equal comparison */
				virtual bool operator==(const Iterator& iter)=0;
				/** not equal comparison */
				virtual bool operator!=(const Iterator& iter)=0;
				/** returns the key for this iterator */		
				virtual const Key& key()=0;
				/** returns the value for this iterator */
				virtual Value& value()=0;
			};
			
			/** returns the first iterator in the map */
			virtual Iterator begin()=0;
			/** return the end of iteration iterator, like eof */
			virtual const Iterator& end()=0;
        };
    }
}

#endif // __SRL_MAP__

