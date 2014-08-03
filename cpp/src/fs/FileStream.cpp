
#include "srl/fs/FileStream.h"
#include "srl/sys/System.h"
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

using namespace SRL;
using namespace SRL::FS;

#ifdef WIN32
#include <io.h>

FileStream::FileStream(HANDLE& hfile, int flags) : _hfile(hfile)
{
    _fd = _open_osfhandle((intptr_t)_hfile, flags);
}
#endif

FileStream::FileStream(int fd, const char* mode) : 
_fd(fd)
{
    _file = ::fdopen(_fd, mode);
#ifdef WIN32
    _hfile = (HANDLE)_get_osfhandle(_fd);
#endif
}

FileStream::FileStream(FILE* file) : 
_file(file)
{
    _fd = ::fileno(file);
}
            
FileStream::~FileStream()
{
    close();
}

bool FileStream::atEnd() const
{
    return (feof(_file) != 0);
}

bool FileStream::canRead(int msec) const
{
#ifdef WIN32
    DWORD bytes_available=0;
    ::PeekNamedPipe(_hfile, NULL, 0, NULL, &bytes_available, NULL);
    return (bytes_available > 0);
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

char FileStream::read()
{
	char car = ::fgetc(_file);
	if (feof(_file) != 0)
		return '\0';
	return car;
}

int FileStream::read(char* data, int len,  int msec)
{
    if (canRead(msec))
    {
        return ::fread(data, sizeof(char), len, _file);
    }
    return 0;
}


// String& FileStream::readLine(String &result, bool strip_eol)
// {
//     char car;
//     while ((car = ::fgetc(_file)) != EOF)
//     {
//         if ((strip_eol) && (car == '\r'))
//         {
//             continue;
//         }
//         
//         if (car == '\n')
//         {
//             if (!strip_eol)
//             {
//                 result += car;
//             }
//             return result;
//         }
//         result += car;
//     }
//     return result;
// }

String& FileStream::readLine(String &result, bool strip_eol)
{
 // create a 1k buffer for reading data into
 result.resize(1024);
 if (::fgets(result._strPtr(), result.allocatedSize()-1, _file))
 {
     size_t len = strlen(result._strPtr());
     // check if we ran out of buffer
     if (len < 1023)
     {
         if (strip_eol)
         {
             // we need to set the size of the string and remove the \n\r
             if ((len > 0) && (result[len-1] == '\n'))
                 --len;
 
             if ((len > 0) && (result[len-1] == '\r'))
                 --len;
            }
         // resize the string to be one less then the total removing the newline
         result.resize(len);
     }
     else
     {
         result.resize(len);
         // still more to read
         String tmp;
         result += readLine(tmp, strip_eol);
     }
     return result;
 }
 result = String::null;
 return result;
}

String FileStream::readLine(bool strip_eol)
{
    String tmp;
    return readLine(tmp, strip_eol);
}

String FileStream::readAll()
{
	String buffer;
	return *readAll(buffer);
}

String* FileStream::readAll(String &in)
{
    if (atEnd())
        return NULL;
	in.resize(256);
	in.resize(::fread(in._strPtr(), sizeof(char), in.allocatedSize(), _file));	
	while (!atEnd())
	{
		int current_size = in.size();
		in.resize(in.allocatedSize()+256);
		in.resize(::fread(&(in._strPtr()[current_size]), sizeof(char), 
				in.allocatedSize() - current_size, _file)+current_size);
	}

	return &in;
}

// WRITER LOGIC

bool FileStream::canWrite(int msec) const
{
#ifdef WIN32
    // TODO WaitForObject win32 canWrite Logic
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

int FileStream::write(const char* data, int len)
{
    return ::fwrite(data, sizeof(char), len, _file);
}


int FileStream::format(const char* fmt, ...)
{
	// local argument list
	va_list list;
	// Start the argument list
	va_start(list, fmt);
	// write to the file
	int bytes_written = vfprintf(_file, fmt, list);
	// free the list
	va_end(list);
	// return the number of bytes written
	return bytes_written;
}

int FileStream::formatLine(const char* fmt, ...)
{
	// local argument list
	va_list list;
	// Start the argument list
	va_start(list, fmt);
	// write to the file
	int bytes_written = vfprintf(_file, fmt, list);
	// free the list
	va_end(list);
	// return the number of bytes written
	return bytes_written + write(EOL_STRING, strlen(EOL_STRING));
}

void FileStream::close()
{
	if (_file)
	{
		fclose(_file);
		_file = NULL;
	}
}
