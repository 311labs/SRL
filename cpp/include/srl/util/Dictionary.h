/**
 * File: Dictionary.h
 * Project: srl3
 * Created by: Ian Starnes on 2006-11-29.
 * 
 * This library is free software; you can redistribute it and/or                 
 * modify it under the terms of the GNU Lesser General Public                    
 * License as published by the Free Software Foundation; either                  
 * version 2.1 of the License, or (at your option) any later version.            
 *                                                                               
 * This library is distributed in the hope that it will be useful,               
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             
 * Lesser General Public License for more details.                               
 *                                                                               
 * You should have received a copy of the GNU Lesser General Public              
 * License along with this library; if not, write to the Free Software           
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.   
 */

#ifndef __SRL_Dictionary__
#define __SRL_Dictionary__

#include "srl/String.h"
#include "srl/NonCopyable.h"
#include "srl/Exceptions.h"

namespace SRL
{
    /** The attempt here is to get away from stl with a more intuitive
    * container framework */
    namespace Util
    {
        /** Dictionary that is a simple key value pairing that is powered by a 
        * linked list for iteration and a RedBlack tree for fast retreival by key
        * it is required that the key has comparison operators (>,<,==) overridden
        */
        template<class Key, class Value, class Hasher=String>
        class Dictionary
        {
        public:
            class Iterator;
            /** Dictionary Entry holds the key and value for the a entry in the dictionary */
            class Entry : private NonCopyable
            {
            friend class Dictionary;
            friend class Iterator;
            public:
                /** returns the key for the entry */
                const Key& key(){ return _key; }
                /** returns the value for the entry */
                Value& value(){ return _value; }
                            
            protected:
                enum COLOR{RED, BLACK};
                /** Construct a new Entry by Key */
                Entry(const Key& key) :
                    _prev(NULL), _next(NULL), _parent(NULL), 
                    _lesser(NULL), _greater(NULL), _color(RED),
                    _key(key), _value(), _hash(Hasher::Hash(key)){}
                /** Construct a new Entry by Key and hash */
                Entry(const Key& key, int hash) :
                    _prev(NULL), _next(NULL), _parent(NULL),
                    _lesser(NULL), _greater(NULL), _color(RED),
                    _key(key), _value(), _hash(hash){}
                                
                /** returns the current hash value for the entry */
                const int& hash(){ return _hash; }
                
                /** set the color of the Red Black Tree Node */
                void _setColor(COLOR color)
                {
                    _color = color;
                    COLOR child_color = RED;
                    
                    if (_color == RED)
                        child_color = BLACK;
                        
                    if (_lesser != NULL)
                        _lesser->_setColor(child_color);
                    if (_greater != NULL)
                        _greater->_setColor(child_color);

                }
                
                /** replace a child entry with a new entry */
                void _replaceChild(Entry *olde, Entry *newe)
                {
                    if (_lesser == olde)
                        _setLesser(newe);
                    else
                        _setGreater(newe);
                }                
                
                /** set the lesser value (updates the parent of the new lesser) */
                void _setLesser(Entry* entry)
                {
                    _lesser = entry;
                    if (entry != NULL)
                        entry->_parent = this;
                }

                /** set the greater value (updates the parent of the new greater) */
                void _setGreater(Entry* entry)
                {
                    _greater = entry;
                    if (entry != NULL)
                        entry->_parent = this;
                }               
                
                /** return 0 to do nothing, -1 to rotate left, 1 to rotate right */ 
                int _checkBalance()
                {
                    // if we have two children then we are ok
                    if ((_greater != NULL) && (_lesser != NULL))
                        return 0;
                    
                    if (_greater != NULL)
                    {
                        if (_greater->_greater != NULL)
                            return -1;
                    }

                    if (_lesser != NULL)
                    {
                        if (_lesser->_lesser != NULL)
                            return 1;
                    }
                    
                    return 0;                    
                }
                
                Entry* grandparent()
                {
                    if (_parent == NULL)
                        return NULL;
                    return _parent->_parent;
                }
                
                Entry* sibling()
                {
                    if (_parent == NULL)
                        return NULL;
                        
                    if (this == _parent->_lesser)
                        return _parent->_greater;

                    return _parent->_lesser;
            
                }

                Entry* successor()
                {
                    Entry *s = NULL;
                    if (_greater != NULL)
                    {
                        s = _greater;
                        while (s->_lesser != NULL)
                        {
                            s = s->_lesser;
                        }
                    }
                    else if (_lesser != NULL)
                    {
                        s = _lesser;
                        while (s->_greater != NULL)
                        {
                            s = s->_greater;
                        }
                    }
                    return s;
                }
                
                int childCount()
                {
                    int count = 0;
                    if (_lesser != NULL)
                        ++count;
                    if (_greater != NULL)
                        ++count;
                    return count;                   
                }
                
                Entry* uncle()
                {
                    Entry* gp = grandparent();
                    if (gp == NULL)
                        return NULL;
                    
                    if (_parent == gp->_lesser)
                        return gp->_greater;

                    return gp->_lesser;                        
                }                
                                
                Entry *_prev,*_next;
                Entry *_parent, *_lesser,*_greater;
                COLOR _color;   

                Key _key;
                Value _value;
                int _hash;
            };
        
            /** Used to iterator thru the dictionary */
            class Iterator
            {
            public:
                /** default constructor requires an entry or null */
                Iterator(Entry* entry):_entry(entry){}
                /** copy constructor */
                Iterator(const Iterator &iter)
                {
                    this->_entry = iter._entry;
                }
                /** assignment operator */
                Iterator& operator=(const Iterator& iter)
                {
                    this->_entry = iter._entry;
                    return *this;
                }
                /** (++iter)moves the iterator forward by one  */
                void operator++()
                {
                    _entry = _entry->_next;
                }
                /** (iter++)moves the iterator forward by one  */
                Iterator operator++(int n)
                {
                    // TODO finish the dictionary iteration operations
                    _entry = _entry->_next;
                    return *this;
                }
                /** (--iter)mover the iterator backwards by one */
                void operator--()
                {
                    _entry = _entry->_prev;
                }
                /** (iter--)mover the iterator backwards by one */
                Iterator operator--(int n)
                {
                    _entry = _entry->_prev;
                    return *this;
                }
                /** comparison */
                bool operator==(const Iterator& iter)
                {
                    if (_entry == iter._entry)
                        return true;
                    return false;
                }
                bool operator!=(const Iterator& iter)
                {
                    if (_entry != iter._entry)
                        return true;
                    return false;
                }           
                const Key& key(){ return _entry->key(); }
                Value& value(){ return _entry->value(); }
            protected:
                Entry* _entry;
            };
        
            /** constructor */
            Dictionary<Key, Value, Hasher>():_size(0),_root(NULL),_first(NULL),_last(NULL),_iter_end(NULL){}
            /** destructor */
            virtual ~Dictionary(){ clear(); }
        
            /** add an object to the map */
            void add(const Key& key, const Value& value)
            {
                if (hasKey(key))
                    throw Errors::InvalidArgumentException("dictionary already contains an entry with this key");
                Entry *entry = new Entry(key);
                entry->_value = value;
                _link(entry);    
                _insert(entry);             
            }
            
            /** remove object from map */
            void remove(const Key& key)
            {
                Entry *entry = _find(key);
                if (entry == NULL)
                    throw Errors::NullPointerException("dictionary entry does not exist");              
                _unlink(entry);
                _remove(entry);
                delete entry;
            }
            
            /** remove item pointed to by the iterator */
            Iterator& remove(Iterator &iter)
            {
                Entry *entry = _find(iter.key());
                if (entry == NULL)
                    throw Errors::NullPointerException("dictionary entry does not exist");              
                ++iter;
                _unlink(entry);
                _remove(entry);
                delete entry;
                return iter;
            } 
            
            /** retrieve the object by key */
            Value& get(const Key& key) const
            {
                Entry *entry = _find(key);
                if (entry == NULL)
                    throw Errors::NullPointerException("dictionary entry does not exist");
                return entry->_value;
            }
            /** return the value object based on a key (if the key doesn't exist a new entry is created) */
            Value& operator[](const Key& key)
            {
                Entry *entry = _find(key);
                if (entry == NULL)
                {
                    entry = new Entry(key);
                    _link(entry);               
                    _insert(entry);
                }
                return entry->value();
            }
                        
            /** does this map contain an entry with the key */
            bool hasKey(const Key& key) const
            {
                return (_find(key) != NULL);
            }
            
            /** does this map contain an entry with this value */
            bool containsValue(const Value& value) const
            {
                Entry *entry = _first;
                while (entry != NULL)
                {
                    if (entry->_value == value)
                        return true;
                    entry = entry->_next;
                }
                return false;
            }
            
            /** returns the number of entries in the dictionary */
            const uint32& size() const{ return _size; }
            /** returns true if the map is empty */
            bool isEmpty() const{ return (size() == 0);}
            
            /** remove all objects */
            void clear()
            {
                Entry *entry = _first;
                Entry *next = NULL;
                while (entry != NULL)
                {
                    next = entry->_next;
                    delete entry;
                    entry = next;
                }
                _size = 0;
                _root = NULL;
                _first = NULL;
                _last = NULL;
                return;             
            }

            /** return the first entry in the dictionary */
            Iterator begin() const{ return Iterator(_first); }
            /** return the last item in the dictionary */
            const Iterator& end() const{ return _iter_end; }
            /** return the iterator for the key if found */
            Iterator find(const Key& key) const 
            {
                Entry *entry = _find(key);
                if (entry == NULL)
                    return _iter_end;
                return Iterator(entry);
            }       

        protected:
            /** holds the size of the dictionary */
            uint32 _size;
            /** holds the root(_first) and the _last iterm inserted */
            Entry *_root, *_first, *_last;
            Iterator _iter_end;
                        
            /** returns a pointer to the entry or null of not found */
            Entry* _find(const Key& key) const
            {
                int key_hash = Hasher::Hash(key);
                Entry *node = _root;
                while (node != NULL)
                {
                    if (key_hash == node->_hash)
                        return node;
                        
                    if (key_hash > node->_hash)
                        node = node->_greater;
                    else
                        node = node->_lesser;
                }
                return NULL;
            }
            
            /** link the new entry into our linked list */
            void _link(Entry *entry)
            {
                // _last represents the last entry added
                if (_last != NULL)
                {
                    // so lets tell the last entry to make our new entry its next
                    _last->_next = entry;
                    // and lets tell our new entry to make our last entry its prev
                    entry->_prev = _last;
                    // now lets set our _last variable to the new entry
                    _last = entry;
                }
                else
                {
                    // both the _first and the _last to the new entry
                    _first = entry;
                    _last = entry;
                }
                ++_size;            
            }
            
            /** unlink the entry from our linked list */
            void _unlink(Entry *entry)
            {
                // update linked list
                if (entry->_prev != NULL)
                    entry->_prev->_next = entry->_next;
                if (entry->_next != NULL)
                    entry->_next->_prev = entry->_prev;
        
                // update the first and last
                Entry *next = entry->_next;
                if (_first == entry)
                    _first = next;
                if (_last == entry)
                    _last = entry->_prev;               
                
                --_size;
            }
                    
            /** insert the item into our tree */
            void _insert(Entry *entry, Entry *parent=NULL)
            {
                // if we do not have a root then this entry is the root
                if (_root == NULL)
                {
                    _root = entry;
                    return;
                }   
                
                // if no parent was given then set the root as the parent
                if (parent == NULL)
                    parent = _root;
                
                // find the best place to insert the entry (simple binary tree insertion)
                while (parent != NULL)
                {
                    if (entry->hash() >= parent->_hash)
                    {
                        if (parent->_greater == NULL)
                        {
                            parent->_setGreater(entry);
                            break;
                        }
                        parent = parent->_greater;
                    }
                    else
                    {
                        if (parent->_lesser == NULL)
                        {
                            parent->_setLesser(entry);
                            break;
                        }
                        parent = parent->_lesser;                       
                    }
                }
                // validate the insertion
                _validateInsert(entry);
                //_balance(_root);
            }

            void _validateInsert(Entry *entry)
            {
                
                while ((entry != _root) && (entry->_parent->_color == Entry::RED))
                {
                    if (entry->grandparent() == NULL)
                        break;
                    // what side of our grandparent do we insert on
                    if (entry->_parent == entry->grandparent()->_lesser)
                    {
                        Entry *y = entry->grandparent()->_greater;
                        // if y is null then it is black
                        if ((y != NULL) && (y->_color == Entry::RED))
                        {
                            entry->_parent->_color = Entry::BLACK;
                            y->_color = Entry::BLACK;
                            entry->grandparent()->_color = Entry::RED;
                            entry = entry->grandparent();
                        }
                        else 
                        {
                            if (entry == entry->_parent->_greater)
                            {
                                entry = entry->_parent;
                                _rotateLeft(entry);
                            }
                        
                            if (entry->_parent != NULL)
                                entry->_parent->_color = Entry::BLACK;
                            
                            if (entry->grandparent() != NULL)
                            {
                                entry->grandparent()->_color = Entry::RED;
                                _rotateRight(entry->grandparent());
                            }
                        }

                    }
                    else
                    {
                        Entry *y = entry->grandparent()->_lesser;
                        // if y is null then it is black
                        if ((y != NULL) && (y->_color == Entry::RED))
                        {
                            entry->_parent->_color = Entry::BLACK;
                            y->_color = Entry::BLACK;
                            entry->grandparent()->_color = Entry::RED;
                            entry = entry->grandparent();
                        }
                        else
                        { 
                            if (entry == entry->_parent->_lesser)
                            {
                                entry = entry->_parent;
                                _rotateRight(entry);
                            }
                        
                            if (entry->_parent != NULL)
                                entry->_parent->_color = Entry::BLACK;
                            
                            if (entry->grandparent() != NULL)
                            {
                                entry->grandparent()->_color = Entry::RED;
                                _rotateLeft(entry->grandparent());
                            }
                        }
                                       
                    }
                }
                _root->_color = Entry::BLACK;
            }
            
            Entry* _rotateRight(Entry *entry)
            {           
                Entry *r = entry;
                Entry *p = entry->_parent;
                Entry *replacement = NULL;
                
                replacement = r->_lesser;
                if (replacement == NULL)
                    return NULL;
                
                if (r == _root)
                {
                    _root = replacement;
                    _root->_parent = NULL; 
                }
                else
                {
                    if (p->_lesser == r)
                    {
                        p->_setLesser(replacement);                  
                    }
                    else
                    {
                        p->_setGreater(replacement);      
                    }
                }
                
                r->_setLesser(replacement->_greater);
                replacement->_setGreater(r);
                return replacement;
            }           

            Entry* _rotateLeft(Entry *entry)
            {            
                Entry *r = entry;
                Entry *p = entry->_parent;
                Entry *replacement = NULL;
                
                replacement = r->_greater;
                if (replacement == NULL)
                    return NULL;
                
                if (r == _root)
                {
                    _root = replacement;
                    _root->_parent = NULL; 
                }
                else
                {
                    if (p->_greater == r)
                    {
                        p->_setGreater(replacement);                  
                    }
                    else
                    {
                        p->_setLesser(replacement);      
                    }
                }
                
                r->_setGreater(replacement->_lesser);
                replacement->_setLesser(r);
                return replacement;
            }
            
            /** remove the entry from out tree */
            void _remove(Entry *entry)
            {
                Entry *y = NULL;
                Entry *x = NULL;
                
                if ((entry->_lesser == NULL)||(entry->_greater == NULL))
                    y = entry;
                else
                    y = entry->successor();
                
                if (y->_lesser != NULL)
                    x = y->_lesser;
                else
                    x = y->_greater;
                
                if (x != NULL)                
                {               
                    x->_parent = y->_parent;
                }
                
                if (y->_parent == NULL)
                    _root = x;
                else
                {
                    if (y == y->_parent->_lesser)
                        y->_parent->_lesser = x;
                    else
                        y->_parent->_greater = x;
                }
            
                
                bool fixflag =  ((y->_color == Entry::BLACK)&& (x != NULL));
                if (y != entry)
                {
                    y->_lesser = entry->_lesser;
                    if (y->_lesser != NULL)
                        y->_lesser->_parent = y;
                    y->_greater = entry->_greater;
                    if (y->_greater != NULL)
                        y->_greater->_parent = y;
                    y->_parent = entry->_parent;
                    y->_color = entry->_color;
                    if (y->_parent != NULL)
                    {
                        y->_parent->_replaceChild(entry, y);
                    }
                    
                    if (entry == _root)
                        _root = y;
                }
                
                if (fixflag)
                {
                    _removeFixTree(x);
                }
            }

            void _removeFixTree(Entry *entry)
            {
                Entry *w = NULL;
                while ((entry != _root) && (entry->_color == Entry::BLACK))
                {
                    if (entry == entry->_parent->_lesser)
                    {
                        w = entry->_parent->_greater;
                        if ((w != NULL)&&(w->_color == Entry::RED))
                        {
                            w->_color = Entry::BLACK;
                            entry->_parent->_color = Entry::RED;
                            _rotateLeft(entry->_parent);
                            w = entry->_parent->_greater;
                        }
                        
                        if ((w != NULL)&&
                            ((w->_lesser == NULL)||(w->_lesser->_color == Entry::BLACK))&&
                            ((w->_greater == NULL)||(w->_greater->_color == Entry::BLACK)))
                        {
                            w->_color = Entry::RED;
                            entry = entry->_parent;
                        }
                        else
                        {
                            if ((w != NULL)&&((w->_greater == NULL)||(w->_greater->_color == Entry::BLACK)))
                            {
                                if (w->_lesser != NULL)
                                    w->_lesser->_color = Entry::BLACK;
                                w->_color = Entry::RED;
                                _rotateRight(w);
                                w = entry->_parent->_greater;
                            }
                            
                            if (w != NULL)
                            {
                                w->_color = entry->_parent->_color;
                                if (w->_parent != NULL)
                                {
                                    entry->_parent->_color = Entry::BLACK;    
                                    if (w->_greater != NULL)                    
                                        w->_greater->_color = Entry::BLACK;
                                    _rotateLeft(entry->_parent);
                                    entry = _root;
                                }
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        w = entry->_parent->_lesser;
                        if ((w != NULL)&&(w->_color == Entry::RED))
                        {
                            w->_color = Entry::BLACK;
                            entry->_parent->_color = Entry::RED;
                            _rotateRight(entry->_parent);
                            w = entry->_parent->_lesser;
                        }
                        
                        if ((w != NULL)&&
                            ((w->_lesser == NULL)||(w->_lesser->_color == Entry::BLACK))&&
                            ((w->_greater == NULL)||(w->_greater->_color == Entry::BLACK)))
                        {
                            w->_color = Entry::RED;
                            entry = entry->_parent;
                        }
                        else
                        {
                            if ((w != NULL)&&((w->_lesser == NULL)||(w->_lesser->_color == Entry::BLACK)))
                            {
                                if (w->_greater != NULL)
                                    w->_greater->_color = Entry::BLACK;
                                w->_color = Entry::RED;
                                _rotateLeft(w);
                                w = entry->_parent->_lesser;
                            }
                            
                            if (w != NULL)
                            {
                                w->_color = entry->_parent->_color;
                                if (w->_parent != NULL)
                                {
                                    entry->_parent->_color = Entry::BLACK;   
                                    if (w->_lesser != NULL)                     
                                        w->_lesser->_color = Entry::BLACK;
                                    _rotateRight(entry->_parent);
                                    entry = _root;
                                }
                            }
                            else
                            {
                                break;
                            }
                        }                        
                    }
                }
                entry->_color = Entry::BLACK;
            }
        
        };

    }
}
#endif

