#ifdef linux
#include "srl/fs/FsObject.h"
#include "srl/util/Vector.h"
#include "srl/sys/Mutex.h"

#include <fcntl.h>
#include <signal.h>

using namespace SRL;
using namespace SRL::FS;

System::Mutex change_mutex;
// TODO this needs to change.. it is auto alocated
Util::Vector<FsObject*>* change_handlers = NULL;

void changeHandler(int signum)
{
    for (int i=0; i<change_handlers->size();++i)
    {
        change_handlers->get(i)->_is_changed = true;
    }
}

void FsObject::startChangeListener()
{
    change_mutex.lock();
        if (change_handlers == NULL)
            change_handlers = new Util::Vector<FsObject*>();    

        if (!change_handlers->hasValue(this))
        {
            _update_stats();
            _is_changed = false;
    		// we only look for changes in the folder
    		String dir_path;
            if (isDir())
    		    dir_path = _full_path;
    		else
    		    dir_path = _path;
    	
    		struct sigaction action;
    		action.sa_handler = changeHandler;
    		sigemptyset( &action.sa_mask );
    		action.sa_flags = SA_RESTART;
    		sigaction( SIGRTMIN, &action, NULL );

    		_fd = ::open( dir_path.c_str(), O_RDONLY );
    		if( _fd < 0 )
    	    {
    	        change_mutex.unlock();
    	        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
    	    }
    		/* Choose the signal I want to receive when the directory content changes */
    		if( fcntl( _fd, F_SETSIG, SIGRTMIN) < 0 )
    		{
    	        change_mutex.unlock();
    	        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
    		}

    		/* Ask for notification when a modification is made in the directory */
    		if( fcntl( _fd, F_NOTIFY, DN_MODIFY|DN_RENAME|DN_DELETE|DN_CREATE|DN_MULTISHOT ) < 0 )
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
            
        if (change_handlers->hasValue(this))
        {
            ::close(_fd);
            change_handlers->remove(this);
        }
    change_mutex.unlock();
}

void FsObject::waitForChange()
{
	// if we are just monitoring a directory
	// then we need not worry about stats
	uint64 last_size = _file_size;
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

#endif // linux


