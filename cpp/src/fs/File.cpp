
#include "srl/fs/File.h"

#include <stdarg.h>
#include <errno.h>
#include <stdio.h>

using namespace SRL;
using namespace SRL::FS;

/** array of io modes */
const char File::__io_modes[12][4] = { "r", "w", "a", "r+", "w+", "a+", "rb", "wb", "ab", "r+b", "w+b", "a+b"};

// TODO should be private not protected
File::File()
{

}

File::~File()
{
	if (_file)
		fclose(_file);
}

File::File(const String &filename, bool newed):
FsObject(filename, newed),
_file(NULL), _io_mode(0),
_file_data_size(0)
{


}

int File::find_io_mode(const char* io_mode)
{
	for (int i=0; i<12; ++i)
	{
		if (String::Compare(File::__io_modes[i], io_mode))
		{
			return i;
		}
	}
	return 0;
}

File::File(const String &filename,  File::MODE io_mode, bool newed):
		FsObject(filename, newed),
_file(NULL), _io_mode(io_mode),
_file_data_size(0)
{
	// open the file with the specified mode
	_openFile();
}

File::File(const String &filename,  const char *io_mode, bool newed):
FsObject(filename, newed),
_file(NULL), _io_mode(find_io_mode(io_mode)),
_file_data_size(0)
{
	// open the file with the specified mode
	_openFile();
}


void File::setIoMode(File::MODE io_mode)
{
	// check if the file is open
	if (!_file)
	{
		throw SRL::Errors::IOException("file must firt be opened before this operation can be performed 'setIoMode()'");
	}
	// check if the io mode is different from the current
	if (io_mode != _io_mode)
	{
		// get the current position
		uint32 pos = position();
		fclose(_file);
		_io_mode = io_mode;
		_openFile();
		setPosition(pos);
	}
}

void File::close()
{
	if (_file)
	{
		fclose(_file);
		_file = NULL;
		if (_is_newed)
			delete this;
	}
}

uint32 File::position() const
{
// 	fpos_t file_pos;
// 	fgetpos(_file, &file_pos);
// 	return (uint32)file_pos;
	return ::ftell(_file);
}

uint32 File::tell() const
{
	return ::ftell(_file);
}

Bool File::atBeginning() const
{
	if (File::tell() == 0)
		return TRUE;
	return FALSE;
}

bool File::atEnd() const
{
	if (feof(_file) == 0)
		return false;
	return true;
}

void File::setPosition(const uint32 &pos)
{
// 	::FSetpos(_file, &pos);
	::fseek(_file, pos, SEEK_SET);
}

void File::seek(const int64 &offset, const int32 origin)
{
	::fseek(_file, offset, origin);
}

void File::gotoBeginning()
{
	rewind();
}

void File::rewind()
{
	::rewind(_file);
}

void File::gotoEnd()
{
	::fseek(_file, 0L, SEEK_END);
}

void File::commit()
{
	::fflush(_file);
}

void File::_openFile()
{
	// make sure its a valid io type
	if ((_io_mode < 0) || (_io_mode > 12))
		throw SRL::Errors::IOException("invalid file mode");
	// now lets open the file
	_file = fopen(fullName().text(), __io_modes[_io_mode]);
	if (_file == NULL )
	{
		int last_error = System::GetLastErrorNumber();
#ifdef WIN32
		switch (last_error)
		{
			case (ERROR_FILE_NOT_FOUND):
			case (ERROR_PATH_NOT_FOUND):
				last_error = 2;
				break;
			case (ERROR_WRITE_PROTECT):
			case (ERROR_ACCESS_DENIED):
				last_error = 5;
				break;
		}
#else
		switch (last_error)
		{
			case (EACCES):
			case (ETXTBSY):
			case (EROFS):
				last_error = 5;
				break;
			case (ENOENT):
			case (ENOSPC):
			case (ENXIO):
				last_error = 2;
				break;
		}
#endif 
		switch (last_error)
		{
			case (5):
				throw SRL::Errors::PermissionDeniedException(
					"Permissions specified by the mode are denied for: """ + fullName() + """");
			case (2):
				throw Errors::FileNotFoundException("File Not Found: """ + fullName() + """");
			default:
				throw SRL::Errors::IOException(String::Format("File failed to open for unknown reasons: errno(%d)",
				 									last_error));
		}
	}
}

const uint64& File::size() const
{
	if (_file_data_size == 0)
	{
		fpos_t pos, endpos;
		// get the current position
		::fgetpos(_file, &pos);
		::rewind(_file);
		//const int64 start = ftell(_file);
		// find the end of the file
		::fseek(_file, 0, SEEK_END);
		::fgetpos(_file, &endpos);
		const int64 stop = ftell(_file);
		// go back to the beginning
		::fsetpos(_file, &pos);

		_file_data_size = stop;
	}
	return _file_data_size; 
}

