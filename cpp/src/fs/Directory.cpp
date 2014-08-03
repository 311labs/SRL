
#include "srl/fs/Directory.h"

using namespace SRL;
using namespace SRL::FS;

Directory::Directory(const String &path, bool newed) :
FsObject(path, newed),
_is_initialized(false),
_children()
{
	refresh();
}

Directory::~Directory()
{  
    _children.clear(true);
}

void Directory::free()
{
    if (_is_newed)
        delete this;
}

Directory* Directory::getParent() const
{
    if (this->path().length() > 1)
        return new Directory(this->path(), true);
    return NULL;
}

bool Directory::hasChild(const String& name)
{
    for (uint32 i=0; i<_children.size();++i)
    {
        if (_children[i]->name() == name)
            return true;
    }
    return false;
}
