
#include "FsTests.h"

#include "srl/fs/FS.h"
#include "srl/sys/System.h"

using namespace SRL;
using namespace SRL::Test;

bool FsTest::apiTest()
{
#ifdef WIN32
    String tmp_path = System::GetEnv("TEMP");
#else
    String tmp_path = "/tmp";
#endif
    if (FS::IsSymLink("/tmp"))
        tmp_path = FS::GetRealPath(tmp_path);

    // check if temp directory exists
    TEST_ASSERT(FS::Exists(tmp_path))
    // make a folder in the temp directory
    String test_dir = tmp_path + "/srl_test";
    if (FS::Exists(test_dir))
        FS::Delete(test_dir);
    TEST_ASSERT(!FS::Exists(test_dir))
    FS::MakeDir(test_dir);    
    TEST_ASSERT(FS::Exists(test_dir))
    TEST_ASSERT(FS::IsDir(test_dir))
    TEST_ASSERT(!FS::IsFile(test_dir))
    // move it
    String test_dir2 = tmp_path + "/srl_test2";
    FS::Move(test_dir, test_dir2, true);
    TEST_ASSERT(FS::Exists(test_dir2))
    TEST_ASSERT(!FS::Exists(test_dir))
    // copy it back
    FS::Copy(test_dir2, test_dir);
    TEST_ASSERT(FS::IsDir(test_dir))
    TEST_ASSERT(FS::Exists(test_dir2))
    TEST_ASSERT(FS::Exists(test_dir))    
    // delete the 2
    FS::Delete(test_dir2);
    TEST_ASSERT(!FS::Exists(test_dir2)) 

#ifndef WIN32    
    // test symlink
    FS::MakeSymLink(test_dir, test_dir2);
    TEST_ASSERT(FS::Exists(test_dir2))
    TEST_ASSERT(FS::IsSymLink(test_dir2))
    FS::Delete(test_dir2);
    TEST_ASSERT(!FS::Exists(test_dir2))
#endif
    // get the user name
    String path_owner = FS::GetUserFromUID(FS::GetUserID(test_dir));
    TEST_ASSERT(path_owner == System::GetUserName())
    
    // change the current working direcotry
    FS::SetCurrentDir(test_dir);
    TEST_ASSERT(FS::GetCurrentDir()==test_dir)
    
    FS::MakeDir("test_folder");
    TEST_ASSERT(FS::Exists(test_dir + "/test_folder"))
    
    FS::Delete(test_dir, true);
    TEST_ASSERT(!FS::Exists(test_dir))
    
    return true;
}

bool FsTest::dirTest()
{
    return true;
}

bool FsTest::txtfileTest()
{
    return true;
}

bool FsTest::fsobjectTest()
{
    return true;
}

bool FsTest::binfileTest()
{
    return true;
}

bool FsTest::monitorTest()
{
    return true;
}

