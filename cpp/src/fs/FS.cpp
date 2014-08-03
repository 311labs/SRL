#include "srl/fs/FS.h"

#include <stdlib.h>

#ifdef WIN32
    #include "shellapi.h"
    #include  <io.h>
	#include "sys/stat.h"
#else
    #include <pwd.h>
    #include <grp.h>
    #include <dirent.h>
    #include <sys/types.h>
    #include <sys/stat.h>
#endif
#include <stdio.h>

using namespace SRL;
using namespace SRL::FS;

char FS::GetSysSeperator()
{
#ifdef WIN32
    return '\\';
#else
    return '/';
#endif
}

String FS::GetCurrentDir()
{
    char buffer[512];
#ifndef WIN32
    if(getcwd(buffer,512)) return String(buffer);
#else
    if(::GetCurrentDirectory(512,buffer)) return String(buffer);
#endif
    return String::null;
}

bool FS::SetCurrentDir(const String &path)
{
#ifdef WIN32
    if (!::SetCurrentDirectory(path.text()))
        throw SRL::Errors::IOException("failed to set current directory!");
#else
    int res = 0;
    if ( (res =::chdir(path.text()))!=0)
        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
#endif
    return true;
}

void FS::ChangeOwner(const String &path, uint32 uid, uint32 gid)
{
#ifdef WIN32

#else
    if (::chown(path.text(), uid, gid) == -1)
        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
#endif
}

uint32 FS::GetUserID(const String &path)
{
    struct stat status;
    if(::stat(path.text(),&status)==0)
    {
        return status.st_uid;
    }
    return 0;
}

uint32 FS::GetGroupID(const String &path)
{
    struct stat status;
    if(::stat(path.text(),&status)==0)
    {
        return status.st_gid;
    }
    return 0;
}

String FS::GetUserFromUID(uint32 uid)
{
#ifndef WIN32
    struct passwd pwd_result;
    struct passwd *pwd_temp;
    char buffer[256];
    if ( getpwuid_r(uid,&pwd_result,buffer,sizeof(buffer),&pwd_temp) == 0)
        return pwd_temp->pw_name;
    throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber())); 
#else
    return "not supported";
#endif

}

String FS::GetGroupFromGID(uint32 gid)
{
    char result[64];
#ifndef WIN32
    ::group grpresult;
    ::group *grp;
    char buffer[1024];
    if(getgrgid_r(gid,&grpresult,buffer,sizeof(buffer),&grp)==0 && grp) return grp->gr_name;
#endif
    sprintf(result,"%u",gid);
    return result;
}

String FS::GetRealPath(const String& path)
{
#ifndef WIN32
    char real_path[PATH_MAX]; 
    return ::realpath(path.text(), real_path);
#else
    return path;
#endif    
}

String FS::GetFileName(const String &path)
{
    // first see if there are any SYSTEM_SEPERATORs
    if (path.count('\\'))
    {
        return path.substr(path.rfind('\\')+1); 
    }
    else if (path.count('/'))
    {
        return path.substr(path.rfind('/')+1);  
    }

    return path;
}

String FS::GetExt(const String &path)
{
    if (path.count('.'))
    {
        return path.substr(path.rfind('.')+1);
    }
    // no extension found
    return String::null;
}

String FS::GetDirName(const String &path)
{
    // first see if there are any SYSTEM_SEPERATORs
    if (path.count('\\'))
    {
        return path.substr(0,path.rfind('\\')); 
    }
    else if (path.count('/'))
    {
        return path.substr(0,path.rfind('/'));  
    }

    return path;
}

bool FS::IsFile(const String &path)
{
#ifdef WIN32
    return !FS::IsDir(path);
#else
    struct stat status;
    return ::stat(path.text(),&status)==0 && S_ISREG(status.st_mode);
#endif

}

bool FS::IsSymLink(const String &path)
{
#ifdef WIN32
    return false;
#else
    struct stat status;
    return ((::lstat(path.text(),&status)==0) && S_ISLNK(status.st_mode) );
#endif

}

bool FS::IsDir(const String &path)
{

#ifdef WIN32
    DWORD atts;
    return ((atts=GetFileAttributes(path.text()))!=0xFFFFFFFF) && (atts&FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat status;
    return ::stat(path.text(),&status)==0 && S_ISDIR(status.st_mode);
#endif

}

bool FS::IsBlockDevice(const String &path)
{
#ifdef WIN32
    return false;
#else
    struct stat status;
    return ::stat(path.text(),&status)==0 && S_ISBLK(status.st_mode);
#endif

}

bool FS::IsCharDevice(const String &path)
{
#ifdef WIN32
    return false;
#else
    struct stat status;
    return ::stat(path.text(),&status)==0 && S_ISCHR(status.st_mode);
#endif

}

bool FS::IsExecutable(const String &path)
{
    if (!FS::Exists(path))
        throw Errors::FileNotFoundException("File Not Found: " + path);
    
#ifndef WIN32
    return (::access(path.text(),FS::EXECUTE)==0);
#else
    SHFILEINFO sfi;
    return (::SHGetFileInfo(path.text(),0,&sfi,sizeof(SHFILEINFO),SHGFI_EXETYPE)!=0);
#endif        
}

FS::Permissions FS::GetPermissions(const String &path)
{
    if (!FS::Exists(path))
        throw Errors::FileNotFoundException("File Not Found: " + path);
                
    struct stat status;
    if (::stat(path.text(),&status)!=0)
        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
    
    uint32 modes = (uint32)status.st_mode;
    return FS::ModesToPermissions(modes);
}

FS::Permissions FS::ModesToPermissions(uint32 &modes)
{   
    Permissions perms;
    // owner
    if (modes & S_IRUSR)
    {
        perms.owners |= FS::READ; 
    }

    if (modes & S_IWUSR)
    {
        perms.owners |= FS::WRITE; 
    }

    if (modes & FS::EXECUTE)
    {
        perms.owners |= FS::EXECUTE; 
    }

    // owner
    if (modes & S_IRGRP)
    {
        perms.groups |= FS::READ; 
    }

    if (modes & S_IWGRP)
    {
        perms.groups |= FS::WRITE; 
    }

    if (modes & S_IXGRP)
    {
        perms.groups |= FS::EXECUTE; 
    }

    // others
    if (modes & S_IROTH)
    {
        perms.others |= FS::READ; 
    }

    if (modes & S_IWOTH)
    {
        perms.others |= FS::WRITE; 
    }

    if (modes & S_IXOTH)
    {
        perms.others |= FS::EXECUTE; 
    }
    return perms;
}

uint32 FS::PermissionsToModes(FS::Permissions &permissions)
{
    uint32 modes = 0;
    // owner
    if (permissions.owners & FS::READ)
    {
        modes |= S_IRUSR; 
    }

    if (permissions.owners & FS::WRITE)
    {
        modes |= S_IWUSR; 
    }

    if (permissions.owners & FS::EXECUTE)
    {
        modes |= S_IXUSR; 
    }

    // group
    if (permissions.groups & FS::READ)
    {
        modes |= S_IRGRP; 
    }

    if (permissions.groups & FS::WRITE)
    {
        modes |= S_IWGRP; 
    }

    if (permissions.groups & FS::EXECUTE)
    {
        modes |= S_IXGRP; 
    }

    // other
    if (permissions.others & FS::READ)
    {
        modes |= S_IROTH; 
    }

    if (permissions.owners & FS::WRITE)
    {
        modes |= S_IWOTH; 
    }

    if (permissions.owners & FS::EXECUTE)
    {
        modes |= S_IXOTH; 
    }
    return modes;
}

String FS::Permissions::asString() const
{
    String res;
    if (owners & FS::READ)
        res += 'r';
    else
        res += '-';

    if (owners & FS::WRITE)
        res += 'w';
    else
        res += '-';
        
    if (owners & FS::EXECUTE)
        res += 'x';
    else
        res += '-';

    if (groups & FS::READ)
        res += 'r';
    else
        res += '-';

    if (groups & FS::WRITE)
        res += 'w';
    else
        res += '-';
        
    if (groups & FS::EXECUTE)
        res += 'x';
    else
        res += '-';
        
    if (others & FS::READ)
        res += 'r';
    else
        res += '-';

    if (others & FS::WRITE)
        res += 'w';
    else
        res += '-';
        
    if (others & FS::EXECUTE)
        res += 'x';
    else
        res += '-';
        


    return res;
}

void FS::MakeDir(const String &path, Permissions *permissions, Bool recursive)
{
    // need to recursivelly create the directory
    if (recursive)
    {
        char sep = FS::GetSysSeperator();
        uint32 i=0;
        String new_path;
        if (path[0] == sep)
        {
            i = 1;
            new_path += sep;
        }
                
        uint32 sections = path.count(sep);
        for(; i<=sections; ++i)
        {
            new_path += path.section(sep, i, true);
            printf("checking %d - %s\n", i, new_path.text());
            if (!FS::Exists(new_path))
	    {
	    	printf("making %s\n", new_path.text());
                FS::MakeDir(new_path, permissions);
            }
	    new_path += sep;
        }
        return;
    }
    
    // TODO: need to have permissions for Win32 directory creation 
#ifndef WIN32
    umask(0000);
    
    uint32 modes = 0;
    if (permissions == NULL)
    {
        Permissions perms;
        perms.groups = FS::READ_WRITE_EXECUTE;
        perms.owners = FS::READ_WRITE_EXECUTE;
        perms.others = FS::NONE;
        modes = FS::PermissionsToModes(perms);
    }
    else
    {
        modes = FS::PermissionsToModes(*permissions);
    }
    if (::mkdir(path.text(), (mode_t)modes)!=0)
        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
#else
    if (::CreateDirectory(path.text(),NULL)==0)
        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));   
#endif
    return;
}

void FS::MakeSymLink(const String &from_path, const String &to_path)
{
    if (!FS::Exists(from_path))
    {
        throw Errors::FileNotFoundException("File Not Found: " + from_path);
    }

#ifndef WIN32
    if (::symlink(from_path.text(), to_path.text())!=0)
        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
#else
    throw SRL::Errors::IOException("NOT IMPLEMENTED");
#endif
        
}

bool FS::Exists(const String &path)
{
    struct stat64 buf;
    return (stat64(path.text(), &buf) == 0);
}

void FS::Move(const String &from_path, const String &to_path, Bool replace_existing)
{
    if (!FS::Exists(from_path))
    {
        throw Errors::FileNotFoundException("File Not Found: " + from_path);
    }

    if (FS::Exists(to_path))
    {
        if (replace_existing)
        {
            FS::Delete(to_path);
        }
        else 
        {
            throw SRL::Errors::IOException(String::Format("Can not move file to '%s' because it already exists! \
                                                     Try setting the 'replace_existing' flag to write over the file", 
                                                     to_path.text()));
        }
    }
    
#ifdef WIN32
    if (!::MoveFile(from_path.text(), to_path.text()))
    {
        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
    }
#else
    if (::rename(from_path.text(), to_path.text()) == -1)
    {
        throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
    }
#endif  

}

void FS::Copy(const String &from_path, const String &to_path, Bool replace_existing)
{
    if (!FS::Exists(from_path))
    {
        throw Errors::FileNotFoundException("File Not Found: " + from_path);
    }

    if (FS::Exists(to_path))
    {
        if (replace_existing)
        {
            FS::Delete(to_path);
        }
        else 
        {
            throw SRL::Errors::IOException(String::Format("Can not copy file to '%s' because it already exists! \
                                                     Try setting the 'replace_existing' flag to write over the file", 
                                                     to_path.text()));
        }
    }   
        
    if (FS::IsDir(from_path))
    {
        Permissions perms = FS::GetPermissions(from_path);
        FS::MakeDir(to_path, &perms);
        // now we need to copy each item in the folder
#ifndef WIN32
        DIR *me = ::opendir(from_path.text());
        if (me == NULL)
            throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
        dirent* child_dir;
        String path;
        while ((child_dir = readdir(me)) != NULL)
        {
            path = "/";
            path += child_dir->d_name; 
            if ((path == "/.")||(path == "/.."))
                continue;
            
            FS::Copy(from_path + path, to_path + path);
        }
        ::closedir(me);
#else
		throw SRL::Errors::IOException("NOT IMPLEMENTED ON WINDOWS!");
#endif        
    }
    else if (FS::IsFile(from_path))
    {
        FILE* in_fp = fopen(from_path.text(), "r");
        FILE* out_fp = fopen(to_path.text(), "w");
        char buf[4096];
        size_t nread;

        while ((nread = fread(buf, sizeof(char), sizeof(buf), in_fp)) > 0) 
        { 
          fwrite(buf, sizeof(char), nread, out_fp); 
        }
        fclose(in_fp);
        fclose(out_fp);
    }
}

uint32 FS::Delete(const String &path, Bool recursive)
{
    if (!FS::Exists(path))
    {
        throw Errors::FileNotFoundException("File Not Found: " + path);
    }
#ifdef WIN32
    // check if the path is a file.. if so delete and exit
    if (!FS::IsDir(path))
    {
        if (!::DeleteFile(path.text()))
        {
            // TODO should probably set error to errno
            throw SRL::Errors::IOException("failed to remove file");
        }
        return 1;
    }
    // first check if the folder is empty
    String search_string = path + "\\*"; 
    uint32 total = 0;
    WIN32_FIND_DATA child; 
    HANDLE file_handle;
    if ((file_handle = ::FindFirstFile( search_string.text(),  &child )) == INVALID_HANDLE_VALUE)
    {
        throw SRL::Errors::IOException(String::Format("failed to get handle for path: %s", path.text()));
    }

    String node_path;
    do
    {
        // ignore '.' and '..' 
        if ((!String::Compare(child.cFileName, ".")) &&
            (!String::Compare(child.cFileName, "..")) )
        {
            ++total;
            if (recursive)
            {
                Delete(path + "\\" + child.cFileName, true);
            }
            else
                break;
        }
    }
    while (::FindNextFile(file_handle, &child));
    ::FindClose(file_handle);
    
    if (!recursive && (total > 0))
    {
        throw SRL::Errors::IOException(String::Format("can not delete directory with children unless recursive flag is true: %s", path.text()));
    }
    ::RemoveDirectory(path.text());
#else
    // check if the path is a file.. if so delete and exit
    if (!FS::IsDir(path))
    {
        if (::remove(path.text()))
        {
            // TODO should probably set error to errno
            throw SRL::Errors::IOException(System::GetErrorString(System::GetLastErrorNumber()));
        }
        return 1;
    }
    // first check if the folder is empty
    DIR *parent;
    uint32 total = 0;
    if ((parent = ::opendir(path.text())) == NULL)
    {
        throw SRL::Errors::IOException(String::Format("failed to get handle for path: %s", path.text()));
    }

    dirent* child;
    String node_path;
    while ((child = readdir( parent)) != NULL)
    {
        // ignore unix '.' and '..' 
        if ((!String::Compare(child->d_name, ".")) &&
            (!String::Compare(child->d_name, "..")) )
        {
            ++total;
            if (recursive)
            {
                Delete(path + "/" + child->d_name, true);
            }
            else
                break;
        }
    }
    
    ::closedir(parent);
    
    if (!recursive && (total > 0))
    {
        throw SRL::Errors::IOException(String::Format("can not delete directory with children unless recursive flag is true: %s", path.text()));
    }
    ::remove(path.text());
#endif
    return 0;
}

