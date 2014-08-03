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

#ifndef __SRL_VECTOR__
#define __SRL_VECTOR__

#include "srl/Exceptions.h"
#include "srl/math/Random.h"

#include <cstring>
#include <cstdlib>

namespace SRL
{
    namespace Util
    {
        /** Vector is a collection most like an array and best used for adding and removing from the end */
        template<class Value>
        class Vector
        {
        public:
            /** Vector Constructor */
            Vector<Value>(uint32 initial_size=32) : _size(0), 
             _init_size(initial_size), _array_size(0), _array(NULL), 
             _end_iter(0, this)
            {
                resize(initial_size);
            }
            
            /** default desctuctor */
            virtual ~Vector()
            {
                resize(0);
            }
        
            /** add an object to the back of the list */
            virtual void add(const Value& value)
            {
                resize(_size+1);
                _array[_size] = value;
                ++_size;
                ++_end_iter;
            }
            
            /** add an object to the back of the list */
            virtual void addAt(const uint32& index, const Value& value)
            {
                if (index >= _size)
                    throw Errors::IndexOutOfBoundsException("index is bigger then the size of the list (addAt)");
                resize(_size+1);
                if (index != (_size - 1))
                    ::memmove(&_array[index+1], &_array[index], (_size-index)*sizeof(Value));                
                _array[index] = value;
                ++_size;
                ++_end_iter;
            }
            /** remove object from list by value */
            virtual void remove(const Value& value)
            {
                removeAt(positionOf(value));
            }
            /** remove object from list by index  */
            virtual void removeAt(const uint32 &index)
            {
                if (index >= _size)
                    throw Errors::IndexOutOfBoundsException("index is bigger then the size of the list (removeAt)");
                // check for simple remove first
                if (index != (_size - 1))
                    ::memmove(&_array[index], &_array[index+1], (_size-(index+1))*sizeof(Value));
                --_size;
                --_end_iter;
                if (_size > _init_size)
                    resize(_size);
            }
            
            /** remove all objects */
            virtual void clear()
            {
                resize(_init_size);
                _size = 0;
                _end_iter._pos = 0;
            }
            
            /** replace the current item at index with the new one returning the old one */
            virtual Value& set(const uint32& index, const Value& value)
            {
                if (index >= _size)
                    throw Errors::IndexOutOfBoundsException("index is bigger then the size of the list (set)");
                Value& old_value = _array[index];
                _array[index] = value;
                return old_value;            
            }
            
            /** retrieve the object by index */
            virtual Value& get(const uint32& index) const
            {
                if (index >= _size)
                    throw Errors::IndexOutOfBoundsException("index is bigger then the size of the list (get)");
                return _array[index];
            }
            /** return the value object based on a key */
            virtual Value& operator[](const uint32& index) const
            {
                return get(index);
            }

            /** position of value, returns -1 if not found */
            virtual int positionOf(const Value& value, uint32 start_index=0) const
            {
                for (uint32 i=start_index; i < _size; ++i)
                {
                    if (_array[i] == value)
                        return i;
                }
                return -1;
            }
            
            //! Tests two XmlRpcValues for equality
            bool operator==(const Vector<Value>& other) const
            {
                // if they are not the same size they are not equal
                if (_size != other._size)
                    return false;
                    
                for (uint32 i=0; i < _size; ++i)
                {
                    if (_array[i] != other._array[i])
                        return false;
                }
                return true;                
            }

            //! Tests two XmlRpcValues for inequality
            bool operator!=(const Vector<Value>& other) const
            {
                return !(this == other);
            }            

            /** does this map contain an entry with this value */
            virtual bool containsValue(const Value& value) const
            {
                return (positionOf(value) >= 0);
            }
            
            /** count the number of occurances of a value the list */
            uint32 count(const Value& value) const
            {
                count = 0;
                for (uint32 i=0; i < _size; ++i)
                {
                    if (_array[i] == value)
                        ++count;
                }
                return count;
            }
                    
            /** returns the size of the map */
            const uint32& size() const{ return _size;}

            void sort(bool descending=true)
            {
                quicksort(0, _size-1);
            }
            
            uint32 partition(uint32 left, uint32 right, uint32 pivot)
            {
                Value pivotValue = _array[pivot];
                // move the pivot to the end
                swap(pivot, right);
                uint32 index = left;
                for( uint32 i = left; i < right; ++i)
                {
                    if (_array[i] <= pivotValue)
                    {
                        if (index != i)
                            swap(index, i);
                        if (index >= right)
                            break;
                        ++index;
                    }
                }
                swap(right, index);
                return index;
            }

            void quicksort(uint32 left, uint32 right)
            {
                if ((right-1) > left)
                {  
                    uint32 pivot = left;                    
                    pivot = partition(left, right, pivot);
                    if ((pivot > left) && (pivot < right))
                    {
                        quicksort(left, pivot-1);
                        quicksort(pivot+1, right);
                    }
                }
            }
         
            // TODO move swap to protected or private
            void swap(uint32 a, uint32 b)
            {
                Value t = _array[a];
                _array[a] = _array[b];
                _array[b] = t;
            }
            
            // NOW OUR INTERATOR INTERFACE
            class Iterator
            {
            friend class Vector<Value>;
            public:
                /** default constructor */
                Iterator(const uint32& pos, const Vector<Value>* vector) : _vector(vector), _pos(pos){};
                /** copy constructor */
                Iterator(const Iterator &iter)
                {
                    this->_pos = iter._pos;
                    this->_vector = iter._vector;
                }
                /** assignment operator */
                Iterator& operator=(const Iterator& iter)
                {
                    this->_pos = iter._pos;
                    this->_vector = iter._vector;
                    return *this;
                }
                /** (++iter)moves the iterator forward by one  */
                void operator++()
                {
                    ++_pos;
                }
                /** (iter++)moves the iterator forward by one  */
                Iterator& operator++(int n)
                {
                    if (n != 0)
                        _pos += n;
                    else
                        _pos++;
                    return *this;
                }
                /** (--iter)mover the iterator backwards by one */
                void operator--()
                {
                    --_pos;
                }
                /** (iter--)mover the iterator backwards by one */
                Iterator& operator--(int n)
                {
                    if (n != 0)
                        _pos -= n;
                    else
                        _pos--;
                    return *this;                   
                }
                /** equal comparison */
                bool operator==(const Iterator& iter)
                {
                    return (_pos == iter._pos);
                }
                /** not equal comparison */
                bool operator!=(const Iterator& iter)
                {
                    return (_pos != iter._pos);
                }
                
                //Value& operator->() const{  return (*_vector)[_pos]; }
                
                /** returns the value for this iterator */
                Value& value() const
                {
                    return (*_vector)[_pos];
                }
            protected:
                /** our parent vector */
                const Vector<Value> *_vector;
                /** the current position of the iterator */
                uint32 _pos;
            };
            
            /** returns the first iterator in the map */
            Iterator begin() const
            {
                return Iterator((uint32)0, this);
            }
            /** return the end of iteration iterator, like eof */
            const Iterator& end() const
            {
                return _end_iter;
            }
            
            /** erase the value at the iterator */
            Iterator erase(Iterator& iter)
            {
                removeAt(iter._pos);
                return iter;
            }
        
        protected:
            /** current number of items in the list */
            uint32 _size, _init_size;
            /** current size allocated in memory */
            uint32 _array_size;
            /** our array */
            Value *_array;
            /** the end iterator */
            Iterator _end_iter;
            
            /** resize the list */
            void resize(uint32 size_needed)
            {
                if (size_needed > 0)
                {
                    if (size_needed < _init_size)
                        size_needed = _init_size;
                    uint32 new_size = ROUNDUP(size_needed+1,32);
                    // exit if we don't need to do anyting
                    if (new_size == _array_size)
                        return;                 

                    if (_array == NULL)
                    {
                        _array = new Value[new_size];
                    }
                    else
                    {
                        Value *new_array = new Value[new_size];
                        if ((new_size > _array_size)&&(_size > 0))
                        {
                            // growing
                            for( uint32 i = 0; i < _size; ++i )
                            {
                                new_array[i] = _array[i];
                            }
                            //::memcpy(new_array, _array, _size-1);
                        }
                            
                        if (_array != NULL)
                            delete[] _array;
                        _array = new_array;                        
                    }
                    _array_size = new_size;
                }
                else if (_array != NULL)
                {
                    _size = 0;
                    _end_iter._pos = 0;
                    _array_size = 0;
                    delete[] _array;
                    _array = NULL;
                }
            }
            
        };
        // POINTER VERSION OF A VECTOR
        /** Vector is a collection most like an array and best used for adding and removing from the end */
        template<class Value>
        class Vector<Value*>
        {
        public:
            /** Vector Constructor */
            Vector<Value*>(uint32 initial_size=32) : _size(0), _array_size(0),
             _init_size(initial_size), _array(NULL), _end_iter(0, this)
            {
                resize(initial_size);
            }
            
            /** default desctuctor */
            virtual ~Vector()
            {
                resize(0);
            }
        
            /** returns an unchanglable for the of the underlying array */
            Value* const* asArray() const {return _array; }
        
            /** add an object to the back of the list */
            virtual void add(Value* value)
            {
                resize(_size+1);
                _array[_size] = value;
                ++_size;
                ++_end_iter;
            }
            
            /** add an object to the back of the list */
            virtual void addAt(const uint32& index, Value* value)
            {
                if (index >= _size)
                    throw Errors::IndexOutOfBoundsException("index is bigger then the size of the list (addAt)");
                resize(_size+1);
                if (index != (_size - 1))
                    ::memmove(&_array[index+1], &_array[index], (_size-index)*sizeof(Value*));                
                _array[index] = value;
                ++_size;
                ++_end_iter;
            }
            /** remove object from list by value */
            virtual void remove(const Value* value)
            {
                removeAt(positionOf(value));
            }
            /** remove object from list by index  */
            virtual void removeAt(const uint32 &index)
            {
                if (index >= _size)
                    throw Errors::IndexOutOfBoundsException("index is bigger then the size of the list (removeAt)");
                // check for simple remove first
                if (index != (_size - 1))
                    ::memmove(&_array[index], &_array[index+1], (_size-(index+1))*sizeof(Value*));
                --_size;
                --_end_iter;
                if (_size > _init_size)
                    resize(_size);
            }
            
            /** 
            * Removes either the last item or the specified item in the vector and returns it 
            */
            Value* pop(int index=-1)
            {
                uint32 pos = 0;
                if (index == -1)
                    pos = _size - 1;
                else
                    pos = static_cast<uint32>(index);
                Value *val = get(pos);
                removeAt(pos);
                return val;
            }
            
            /** remove all objects (DANGER DO_DELETE POINTERS ONLY!!) */
            virtual void clear(bool do_delete=false)
            {
                if (do_delete)
                {
                    // TODO BAD DELETE!!
                    for (uint32 i=0;i<_size;++i)
                        delete _array[i];
                }
                _size = 0;
                resize(_init_size);
                _end_iter._pos = 0;
            }
            
            /** replace the current item at index with the new one returning the old one */
            virtual Value* set(const uint32& index, Value* value)
            {
                if (index >= _size)
                    throw Errors::IndexOutOfBoundsException("index is bigger then the size of the list (set)");
                Value* old_value = _array[index];
                _array[index] = value;
                return old_value;            
            }
            
            /** retrieve the object by index */
            virtual Value* get(const uint32& index) const
            {
                if (index >= _size)
                    throw Errors::IndexOutOfBoundsException("index is bigger then the size of the list (get)");
                return _array[index];
            }
            /** return the value object based on a key */
            virtual Value* operator[](const uint32& index) const
            {
                return get(index);
            }
            
            //! Tests two XmlRpcValues for equality
            bool operator==(const Vector<Value*>& other) const
            {
                // if they are not the same size they are not equal
                if (_size != other._size)
                    return false;
                    
                for (uint32 i=0; i < _size; ++i)
                {
                    if (_array[i] != other._array[i])
                        return false;
                }
                return true;                
            }

            //! Tests two XmlRpcValues for inequality
            bool operator!=(const Vector<Value*>& other) const
            {
                return !(this == other);
            }       

            /** position of value, returns -1 if not found */
            virtual int positionOf(const Value* value, uint32 start_index=0) const
            {
                for (uint32 i=start_index; i < _size; ++i)
                {
                    if (_array[i] == value)
                        return i;
                }
                return -1;
            }

            /** does this map contain an entry with this value */
            virtual bool hasValue(const Value* value) const
            {
                return (positionOf(value) >= 0);
            }

            /** does this map contain an entry with this value */
            virtual bool containsValue(const Value* value) const
            {
                return (positionOf(value) >= 0);
            }
            
            /** count the number of occurances of a value the list */
            uint32 count(const Value* value) const
            {
                count = 0;
                for (uint32 i=0; i < _size; ++i)
                {
                    if (_array[i] == value)
                        ++count;
                }
                return count;
            }
                    
            /** returns the size of the map */
            const uint32& size() const{ return _size;}
            
            // NOW OUR INTERATOR INTERFACE
            class Iterator
            {
                friend class Vector<Value*>;
            public:
                /** default constructor */
                Iterator(const uint32& pos, const Vector<Value*> *vector) : _vector(vector), _pos(pos)
                {

                }
                /** copy constructor */
                Iterator(const Iterator &iter)
                {
                    this->_pos = iter._pos;
                    this->_vector = iter._vector;
                }
                /** assignment operator */
                Iterator& operator=(const Iterator& iter)
                {
                    this->_pos = iter._pos;
                    this->_vector = iter._vector;
                    return *this;
                }
                /** (++iter)moves the iterator forward by one  */
                void operator++()
                {
                    ++_pos;
                }
                /** (iter++)moves the iterator forward by one  */
                Iterator& operator++(int n)
                {
                    if (n != 0)
                        _pos += n;
                    else
                        _pos++;
                    return *this;
                }
                /** (--iter)mover the iterator backwards by one */
                void operator--()
                {
                    --_pos;
                }
                /** (iter--)mover the iterator backwards by one */
                Iterator& operator--(int n)
                {
                    if (n != 0)
                        _pos -= n;
                    else
                        _pos--;
                    return *this;                   
                }
                /** equal comparison */
                bool operator==(const Iterator& iter)
                {
                    return (_pos == iter._pos);
                }
                /** not equal comparison */
                bool operator!=(const Iterator& iter)
                {
                    return (_pos != iter._pos);
                }
                
                Value* operator->() const{  return (*_vector)[_pos]; }
                /** returns the value for this iterator */
                Value* value() const
                {
                    return (*_vector)[_pos];
                }
            protected:
                /** our parent vector */
                const Vector<Value*> *_vector;
                /** the current position of the iterator */
                uint32 _pos;
            };
            
            /** returns the first iterator in the map */
            Iterator begin() const
            {
                return Iterator((uint32)0, this);
            }
            /** return the end of iteration iterator, like eof */
            const Iterator& end() const
            {
                return _end_iter;
            }

            void sort()
            {
                quicksort(0, _size-1);
            }
            
            /** erase the value at the iterator */
            Iterator erase(Iterator& iter)
            {
                removeAt(iter._pos);
                return iter;
            }            
            

            
            uint32 partition(uint32 left, uint32 right, uint32 pivot)
            {
                Value* pivotValue = _array[pivot];
                // move the pivot to the end
                swap(pivot, right);
                uint32 index = left;
                // for( uint32 i = left; i < right; ++i)
                // {
                //     if (_array[i] <= pivotValue)
                //     {
                //         if (index != i)
                //             swap(index, i);
                //         if (index >= right)
                //             break;
                //         ++index;
                //     }
                // }
                // swap(right, index);
                return index;
            }

            void quicksort(uint32 left, uint32 right)
            {
                // if ((right-1) > left)
                // {  
                //     uint32 pivot = left;
                //     Console::format("sorting: left: %d  right: %d  pivot: %d", left, right, pivot);                       
                //     pivot = partition(left, right, pivot);
                //     Console::formatLine(" index: %d", pivot);
                //     if ((pivot > left) && (pivot < right))
                //     {
                //         quicksort(left, pivot-1);
                //         quicksort(pivot+1, right);
                //     }
                // }
            }
            
            // TODO move swap to protected or private
            void swap(uint32 a, uint32 b)
            {
                Value* t = _array[a];
                _array[a] = _array[b];
                _array[b] = t;
            }
            
        
        protected:
            /** current number of items in the list */
            uint32 _size;
            /** current size allocated in memory */
            uint32 _array_size, _init_size;
            /** our array */
            Value **_array;
            /** the end iterator */
            Iterator _end_iter;
        
            /** resize the list */
            void resize(uint32 size_needed)
            {               
                if (size_needed > 0)
                {
                    if (size_needed < _init_size)
                        size_needed = _init_size;                    
                    uint32 new_size = ROUNDUP(size_needed+1,16);
                    // exit if we don't need to do anyting
                    if (new_size == _array_size)
                        return;                 
                    _array_size = new_size;
                    
                    if (_array == NULL)
                    {
                        _array = (Value**)::malloc(new_size*sizeof(Value*)); 
                    }
                    else
                    {
                        _array = (Value**)::realloc(_array, new_size*sizeof(Value*));
                    }
                    _array[_size] = NULL;
                }
                else if (_array != NULL)
                {
                    _size = 0;
                    _end_iter._pos = 0;
                    _array_size = 0;
                    ::free(_array);
                    _array = NULL;
                }
            }
            
        };        
        
    }
}

#endif // __SRL_VECTOR__

