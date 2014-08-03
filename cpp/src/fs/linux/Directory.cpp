#include "srl/fs/Directory.h"

#if defined(linux) || defined(OSX)

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace SRL;
using namespace SRL::FS;

void Directory::refresh()
{
	_is_initialized = true;
	DIR *me;
	if ((me = ::opendir(this->fullName().text())) == NULL)
        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));

	// create a temporary array of children namespace
	Util::Vector<String> new_list;

	dirent* child_dir;
	while ((child_dir = readdir( me)))
	{
	    String child_name = child_dir->d_name;
	    if (!hasChild(child_name))
	    {
	        String full_path = this->fullName() + "/";
	        full_path += child_dir->d_name;
		    _children.add(new FsObject(full_path, true));
	    }
	    new_list.add(child_name);
	}
	::closedir(me);

	// check if any one was deleted
	Util::Vector<FsObject*> remove_list;
	
	for (uint32 i=0; i<_children.size();++i)
	{
	    FsObject *child = _children[i];
	    if (!new_list.containsValue(child->name()))
	    {
	        remove_list.add(child);
	    }
	}
	
	for (uint32 i=0; i<remove_list.size();++i)
    {
        FsObject *child = remove_list[i];
	    _children.remove(remove_list[i]);
	    child->free();
	}
}

#endif

