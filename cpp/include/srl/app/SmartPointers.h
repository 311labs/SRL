#ifndef _SMARTPOINTERS_H_
#define _SMARTPOINTERS_H_

#include "srl/sys/Mutex.h"

namespace SRL
{
    template<class T> class ScopedPointer
    {
    public:
        typedef T _class;
        
        /** 
        * default contructor 
        * explicit makes the call T a = ptr illegal
        */
        explicit ScopedPointer(T *ptr): _ptr(ptr){}
        /** destructor */
        ~ScopedPointer(){ delete _ptr; }
        
        /** return a dereferenced pointer */
        T& operator*() const{ return *_ptr; }
        /** return our pointer, makes this wrapper transparent */
        T* operator->() const{ return _ptr; }
        /** returns the pointer for comparison */
        T* getPointer() const{ return _ptr; }
        /** returns true if the pointer is NULL */
        bool isNULL() const { return _ptr == NULL; }        
    private:
        /** don't let the pointer be copied */
        ScopedPointer(const ScopedPointer& ptr);
        ScopedPointer& operator=(const ScopedPointer& ptr);
        typedef ScopedPointer<T> _type;
        T *_ptr;
    
    };
    
    // TODO
    // a better way to do a shared pointer is to make the sharedpointer have the same logic
    // of scopedpointer that wraps the ReferenceCountedPointer
    
    template<class T> class SharedPointer
    {
    public:
        explicit SharedPointer(): _ptr(new T()), _count(1){}
        explicit SharedPointer(T *ptr): _ptr(ptr), _count(1){}
        ~SharedPointer(){ if (_ptr!=NULL) delete _ptr; }

        SharedPointer(const SharedPointer<T>& ptr) : _ptr(ptr._ptr), _count(ptr._count){}
        
        SharedPointer<T>& operator=(const SharedPointer<T>& ptr)
        {
            // FIXME what if the old pointer is still being used
            _ptr = ptr._ptr;
            _count = ptr._count;
        }

        /** returns a reference to this pointer */
        SharedPointer<T>& getReference()
        { 
            _count_lock.lock();
            ++_count;
            _count_lock.unlock(); 
            return *_ptr; 
        }

        /** release a reference to this object */
        void releaseReference()
        {
            if (_count == 0)
                return;
            _count_lock.lock();
            --_count;
            if ((_count == 0) && (_ptr != NULL))
            {
                delete _ptr;
                _ptr = NULL;
            }
            _count_lock.unlock();
        }

        /** returns the current reference count */
        const uint32& references() const { return _count; }

        /** return a dereferenced pointer */
        T& operator*() const{ return *_ptr; }
        /** return our pointer, makes this wrapper transparent */
        T* operator->() const{ return _ptr; }
        /** returns the pointer for comparison */
        T* getPointer() const{ return _ptr; }
        /** returns true if the pointer is NULL */
        bool isNULL() const { return _ptr == NULL; }
    private:
        typedef SharedPointer<T> _type;
        T *_ptr;
        uint32 _count;
        System::Mutex _count_lock;
    };
    
    
    
    
} /* SRL */



#endif /* _SMARTPOINTERS_H_ */
