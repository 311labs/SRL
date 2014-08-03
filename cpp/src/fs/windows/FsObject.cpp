#ifdef WIN32
#include "srl/fs/FsObject.h"

using namespace SRL;
using namespace SRL::FS;


void FsObject::startChangeListener()
{
	_update_stats();
	_is_changed = false;
	// we first only look at changes in a folder
	String dir_path;
	if ((!isDir()) && (Exists(_full_path)))
	{
		dir_path = _path;
	}
	else
	{
		dir_path = _full_path;
	}

	// lets create the handle to use for size change notifications
	_change_handle = ::FindFirstChangeNotification((LPCSTR)dir_path.c_str(),
							false, FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_FILE_NAME);
	if (_change_handle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	
	return;
}
 
void FsObject::stopChangeListener()
{
	::FindCloseChangeNotification(_change_handle);
}

void FsObject::waitForChange()
{
	// loop until problems
	DWORD	result = 0;
	uint64 last_size = _file_size;

	if (isDir())
	{
		// this should sleep until our event has happened
		if (::WaitForSingleObject(_change_handle, INFINITE) == WAIT_FAILED)
			throw SRL::Errors::IOException("WaitForSingleObject call failed!");

		// reset the change notificationBase
		if (::FindNextChangeNotification(_change_handle) == FALSE )
			throw SRL::Errors::IOException("FindNextChangeNotification call failed!");
		
		_update_stats();
	}
	else if (!Exists(_full_path))
	{
		while (true)
		{
			// this should sleep until our event has happened
			if (::WaitForSingleObject(_change_handle, INFINITE) == WAIT_FAILED)
				throw SRL::Errors::IOException("WaitForSingleObject call failed!");
		
			// reset the change notificationBase
			if (::FindNextChangeNotification(_change_handle) == FALSE )
				throw SRL::Errors::IOException("FindNextChangeNotification call failed!");
				
			if (Exists(_full_path))
				break;
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
		while (true)
		{
			// this should sleep until our event has happened
			if (::WaitForSingleObject(_change_handle, INFINITE) == WAIT_FAILED)
				throw SRL::Errors::IOException("WaitForSingleObject call failed!");
		
			// reset the change notificationBase
			if (::FindNextChangeNotification(_change_handle) == FALSE )
				throw SRL::Errors::IOException("FindNextChangeNotification call failed!");

			// make sure the file still exists
			// TODO this is sort of pointless because if we have the file open for tailing
			// i don't think anyone can delete it
			if (!Exists(fullName()))
				throw SRL::Errors::IOException("File no longer exists!");
			// update the current file stats
			_update_stats();
			if (last_size != _file_size)
			{
				// the file has changed... return
				break;
			}
		}
		
	}
}

#endif // win32


