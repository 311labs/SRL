#include "srl/fs/BinaryFile.h"
#include <stdio.h>

using namespace SRL;
using namespace SRL::FS;

bool BinaryFile::canRead(int msec) const
{
#ifdef WIN32
    return !atEnd();
#else
    if (::feof(_file) != 0)
        return false;
	// setup our variables
	int result=SRL::INVALID; fd_set files;
	FD_ZERO(&files); FD_SET( _fd, &files );
	
	if( msec == -1 )
	{
		result = ::select( _fd+1, &files, 0, 0, 0 );
	}
	else 
	{
		timeval tv; tv.tv_sec = msec/1000; tv.tv_usec = (msec*1000)%1000000;
		result = ::select( _fd+1, &files, 0, 0, &tv );
	}
	// check for time out and throw an exception
	if (result == -1)
		throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));

	return result && (result != SRL::INVALID);
#endif
}

SRL::byte BinaryFile::read()
{
	return (SRL::byte)::fgetc(_file);    
}

int BinaryFile::read(void* obj, uint32 objsize, uint32 arrysize)
{
	return ::fread(obj, objsize, arrysize, _file);
}

int BinaryFile::write(const void* obj, uint32 objsize, uint32 arrysize)
{
	return ::fwrite(obj, objsize, arrysize, _file);
}

bool BinaryFile::canWrite(int msec) const
{
#ifdef WIN32
    return true;
#else
	// setup our variables
	int result=SRL::INVALID; fd_set files;
	FD_ZERO(&files); FD_SET( _fd, &files );
	
	if( msec == SRL::FOREVER )
		result = ::select( _fd+1, 0, &files, 0, 0 );
	else 
	{
		timeval tv; tv.tv_sec = msec/1000; tv.tv_usec = (msec*1000)%1000000;
		result = ::select( _fd+1, 0, &files, 0, &tv );
	}
	// check for time out and throw an exception
	if (result == -1)
		throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));

	return result && (result != SRL::INVALID);
#endif
}

void BinaryFile::close()
{
	if (_file)
	{
		fclose(_file);
		_file = NULL;
		if (_is_newed)
			delete this;
	}
}

