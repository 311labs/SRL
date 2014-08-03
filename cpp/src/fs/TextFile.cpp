
#include "srl/fs/TextFile.h"

#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

using namespace SRL;
using namespace SRL::FS;

bool TextFile::canRead(int msec) const
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

char TextFile::read()
{
	char car = ::fgetc(_file);
	if (feof(_file))
		return '\0';
	return car;
}


int TextFile::read(String &buf, int len, int msec)
{
	buf.resize(len);

    int received = ::fread((char*)buf._strPtr(), sizeof(char), len, _file);
	buf.resize(received);
	return received;        

    return 0;
}

int TextFile::read(char* data, int len,  int msec)
{
    if (canRead(msec))
    {
        return ::fread(data, sizeof(char), len, _file);
    }
    return 0;
}

String& TextFile::readLine(String &result, bool strip_eol)
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

String TextFile::readLine(bool strip_eol)
{
    String tmp;
    return readLine(tmp, strip_eol);
}

String TextFile::readAll()
{
	String buffer;
	return *readAll(buffer);
}

String* TextFile::readAll(String &in)
{
	rewind();
	
	if (size() > 0)
	{
		in.resize(size());
		in.resize(::fread(in._strPtr(), sizeof(char), in.allocatedSize(), _file));	
	}
	else
	{
		in.resize(256);
		in.resize(::fread(in._strPtr(), sizeof(char), in.allocatedSize(), _file));	
		while (!atEnd())
		{
			int current_size = in.size();
			in.resize(in.allocatedSize()+256);
			in.resize(::fread(&(in._strPtr()[current_size]), sizeof(char), 
					in.allocatedSize() - current_size, _file)+current_size);
		}
	}

	return &in;
}

String TextFile::tail()
{
    String tmp;
    return *tail(tmp);
}

String* TextFile::tail(String &result)
{
	// TODO this can be written better by just reading the diff in size
	// lets check for size diff here before calling goto end
	uint64 last_size = FsObject::size();
	// update the stats again to verify things haven't changed
	_update_stats();
	if (last_size == FsObject::size())
	{
		gotoEnd();
	}

	waitForChange();
	//System::Sleep(1);
	result = "";
	uint64 pos = this->position();
	while (pos < FsObject::size())
	{
		System::Sleep(1);
		if (result.length() == 0)
		    this->readLine(result, false);
		else
		    result += this->readLine(false);
		pos = this->position();
	}
	return &result;    
}

// WRITER LOGIC

bool TextFile::canWrite(int msec) const
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

int TextFile::write(const char* data, int len)
{
    return ::fwrite(data, sizeof(char), len, _file);
}


int TextFile::format(const char* fmt, ...)
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

int TextFile::formatLine(const char* fmt, ...)
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

void TextFile::scanf(const char* format, ...)
{
	va_list args;
	va_start(args,format);
	this->vscanf(format,args);
	va_end(args);
}

void TextFile::vscanf(const char* format, va_list args)
{
// TODO win32 support
// TODO test
#ifndef WIN32
	vfscanf(_file, format, args);
#endif
}

void TextFile::close()
{
	if (_file)
	{
		fclose(_file);
		_file = NULL;
		if (_is_newed)
			delete this;
	}
}


// PUBLIC HELPER FUNCTIONS
TextFile* FS::CreateTextFile(const String &filename, bool overwrite)
{
	// check if we can just overwrite existing file
	if ((!overwrite) && (FS::Exists(filename)))
	{
		throw SRL::Errors::IOException("File already exists!");
	}
	return new TextFile(filename, File::ASCII_WRITE, true);
}

TextFile* FS::OpenTextFile(const String &filename, bool for_writing)
{
	if (for_writing)
	{
		return new TextFile(filename, File::ASCII_READ_APPEND, true);
	}
	return new TextFile(filename, File::ASCII_READ, true);
}

void FS::WriteTextFile(const SRL::String &filename, const SRL::String &text, bool overwrite)
{
	// check if we can just overwrite existing file
	if ((!overwrite) && (FS::Exists(filename)))
	{
		throw SRL::Errors::IOException("File already exists!");
	}
	TextFile file(filename, File::ASCII_WRITE);
	file.write(text);
	file.close();
}

void FS::AppendToTextFile(const String &filename, const String &text)
{
	TextFile file(filename, File::ASCII_APPEND);
	file.write(text);
	file.close();
}

void FS::CatTextFile(const String &filename, String *result)
{
	TextFile file(filename, File::ASCII_READ);
	file.readAll(*result);
	file.close();
}

String FS::Cat(const String &filename)
{
    String result;
	TextFile file(filename, File::ASCII_READ);
	file.readAll(result);
	file.close();
    return result;
}

void FS::ScanTextFile(const String&filename, const char *format, ...)
{
	va_list args;
	va_start(args,format);
	TextFile file(filename, File::ASCII_READ);
	file.vscanf(format,args);
	va_end(args);
}
