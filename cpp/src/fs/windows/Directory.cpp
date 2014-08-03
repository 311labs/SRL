#include "srl/fs/Directory.h"

#ifdef WIN32

#include "srl/sys/System.h"
#include "srl/Console.h"

using namespace SRL;
using namespace SRL::FS;

// TAKEN FROM MSDN EXAMPLE (windows blah)
void Directory::refresh()
{
    WIN32_FIND_DATA file_data;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    String path_all = this->fullName();
    // windows wants requires the * to show all files in the directory /* 
    // how brilliant, wow, NOT, stupid windows
    if (isDir())
    {
        path_all += "\\*";
    }

     // Find the first file in the directory.
    Util::Vector<String> new_list;
    
    hFind = FindFirstFile(path_all.text(), &file_data);
    if (hFind == INVALID_HANDLE_VALUE)
	{

		return;
	}

    String full_path = this->fullName() + "/";
    full_path += file_data.cFileName;
    _children.add(new FsObject(full_path, true));
    new_list.add(file_data.cFileName);
    
    while (FindNextFile(hFind, &file_data) != 0) 
    {
        String child_name = file_data.cFileName;
        if (!hasChild(child_name))
        {
            String full_path = this->fullName() + "/";
            full_path += child_name;

            _children.add(new FsObject(full_path, true));
			new_list.add(file_data.cFileName);
        }
    }

    FindClose(hFind);

    // check if any one was deleted
    Util::Vector<FsObject*> remove_list;
    
    for (int i=0; i<_children.size();++i)
    {
        FsObject *child = _children[i];
        if (!new_list.containsValue(child->name()))
        {
            remove_list.add(child);
        }
    }
    
    for (int i=0; i<remove_list.size();++i)
    {
        FsObject *child = remove_list[i];
        _children.remove(remove_list[i]);
        child->free();
    }

    return;
}

#endif

