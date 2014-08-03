
#include "srl/fs/FsObject.h"
#include <sys/stat.h>

#ifdef WIN32
	#include "shellapi.h"
	#include  <io.h>
#endif

using namespace SRL;
using namespace SRL::FS;

FsObject::FsObject(const String &path, bool is_newed) :
_is_changed(false),
_is_newed(is_newed),
_full_path(path),
_name(FS::GetFileName(path)),
_path(FS::GetDirName(path)),
_permissions(),
_st_mode(0),
_file_size(0L),
_file_uid(0), _file_gid(0),
_did_stats(false)
{
	_permissions.owners = 0;
	_permissions.groups = 0;
	_permissions.others = 0;
}

FsObject::~FsObject()
{

}

void FsObject::free()
{
	if (_is_newed)
		delete this;
}

const String& FsObject::name() const
{
	return _name;
}

const String& FsObject::path() const
{
	return _path;
}

const String& FsObject::fullName() const
{
	return _full_path;
}

AccessModes FsObject::myAccess() const
{
    uint32 am = 0;
	if (isReadable())
	{
		am |= FS::READ;
	}

	if (isWritable())
	{
		am |= FS::WRITE;
	}

	if (isExecutable())
	{
		am |= FS::EXECUTE;
	}
	return (AccessModes)am;
}

bool FsObject::isReadable() const
{
	return (::access(_full_path.text(),FS::READ)==0);
}

bool FsObject::isWritable() const
{
	return (::access(_full_path.text(),FS::WRITE)==0);
}

bool FsObject::isExecutable() const
{
    return FS::IsExecutable(_full_path.text());
}

const FS::Permissions& FsObject::permissions() const
{
	if (!_did_stats)
		_update_stats();
	return _permissions;
}


const DateTime& FsObject::createdDate() const
{
	if (!_did_stats)
		_update_stats();
	return _dt_created;
}

const DateTime& FsObject::modifiedDate() const
{
	if (!_did_stats)
		_update_stats();
	return _dt_modified;
}

const DateTime& FsObject::accessedDate() const
{
	if (!_did_stats)
		_update_stats();
	return _dt_accessed;
}

const uint32& FsObject::uid() const
{
	if (!_did_stats)
		_update_stats();

	return _file_uid;
}

const uint32& FsObject::gid() const
{
	if (!_did_stats)
		_update_stats();

	return _file_gid;
}

const uint64& FsObject::size() const
{
	if (!_did_stats)
		_update_stats();

	return _file_size;
}

bool FsObject::isDir()
{
	return FS::IsDir(_full_path);
}

bool FsObject::isFile()
{
	return FS::IsFile(_full_path);
}

bool FsObject::isSymLink()
{
	return FS::IsSymLink(_full_path);
}

void FsObject::_update_stats() const
{
	if (!Exists(_full_path))
		return;
		
	struct stat status;
	if (::stat(_full_path.text(),&status)!=0)
	{
		// TODO added better error details
		throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
	}
	
	_dt_created = DateTime(status.st_ctime);
	_dt_modified = DateTime(status.st_mtime);
	_dt_accessed = DateTime(status.st_atime);
	_file_size = status.st_size;
	_file_uid = status.st_uid;
	_file_gid = status.st_gid;
	_st_mode = (uint32)status.st_mode;
	_permissions = FS::ModesToPermissions(_st_mode);
	_did_stats = true;
}

