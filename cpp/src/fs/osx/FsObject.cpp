
#include "srl/fs/FsObject.h"
#include "srl/util/Vector.h"
#include "srl/sys/Mutex.h"

#ifdef OSX

#include <sys/event.h>

using namespace SRL;
using namespace SRL::FS;

System::Mutex change_mutex;
// TODO this needs to change.. it is auto alocated
Util::Vector<FsObject*>* change_handlers = NULL;

void changeHandler(int signum)
{
    for (uint32 i=0; i<change_handlers->size();++i)
    {
        change_handlers->get(i)->_is_changed = true;
    }
}

void FsObject::startChangeListener()
{
    change_mutex.lock();
        if (change_handlers == NULL)
            change_handlers = new Util::Vector<FsObject*>();    

        if (!change_handlers->containsValue(this))
        {
            _update_stats();
            _is_changed = false;

     		// we first only look at changes in a folder
     		String dir_path = _full_path;

            _kq = kqueue();

             if (_kq == -1)
             {
     	        change_mutex.unlock();
     	        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
             }   

     		_fd = ::open( dir_path.c_str(), O_RDONLY );
     		if( _fd < 0 )
     		{
    	        change_mutex.unlock();
    	        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
     		}           
            
            change_handlers->add(this);    
        }
    change_mutex.unlock();
}
 
void FsObject::stopChangeListener()
{
    change_mutex.lock();
        if (change_handlers == NULL)
            return;
            
        if (change_handlers->containsValue(this))
        {
    		::close(_kq);
    		::close(_fd);            
            change_handlers->remove(this);
        }
    change_mutex.unlock();
}

void FsObject::waitForChange()
{
	// if we are just monitoring a directory
	// then we need not worry about stats
	//uint64 last_size = _file_size;
	_is_changed = false;
	
	if (isDir())
	{
		while (!_is_changed)
		{
			pause();
		}
		_update_stats();
	}
	else if (!Exists(_full_path))
	{
		while (!_is_changed)
		{
			pause();
			if (!Exists(_full_path))
				_is_changed = false;
		}
		_update_stats();
	}
	else
	{
		// now we are monitoring a file
		_is_changed = false;
		uint64 last_size = _file_size;
		// update the stats again to verify things haven't changed
		_update_stats();
		if (last_size != _file_size)
		{
			// things have change, return
			return;
		}	
		// since change event monitors a path we need to
		// stay in the loop until our file has changed
		while (!_is_changed)
		{
			// this will sleep until our event fires
			pause();
			// make sure the file still exists
			if (!Exists(fullName()))
				throw SRL::Errors::IOException("File no longer exists!");
			// update the current file stats
			_update_stats();
			if (last_size == _file_size)
			{
				_is_changed = false;
			}
		}
		
	}
}



#endif // osx



