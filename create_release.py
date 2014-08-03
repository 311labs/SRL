#!/usr/bin/python

import sys, os, platform, commands

SERVER_PATH="/projects/srl/www/pub/release"

def display(msg, newline=True, silent=False):
    if silent:
        return
    if newline:
        msg += '\n'
    sys.stdout.write(msg)
    sys.stdout.flush()

def getVersion(cmd):
    v = commands.getoutput(cmd)
    v = v[v.find('.')-1:].strip()
    return v

def error(msg, newline=True):
    if newline:
        msg += '\n'
    sys.stderr.write(msg)

RELEASE_DIR = "release"

cur = platform.system().lower()
if cur.startswith("cygwin"):
    PLATFORM = "WINDOWS"
    VERSION = getVersion("./bin/proclist -v | grep Version")
    LIB_FILES = ["lib/srl.lib", "lib/jsrl.lib", "lib/jsrl.jar"]
    EXE_FILES = ["bin/proclist.exe", "bin/nettool.exe", "bin/srl.dll", "bin/jsrl.dll"]
    ZIP_CMD = "zip -r"
    ZIP_PATH = "%s/windows/" % SERVER_PATH
    ZIP_CUR = "srl_current.zip"    
    ZIP_FILE = "srl_win32_%s.zip" % VERSION.replace('.', '_')
elif cur.startswith("linux"):
    PLATFORM = "LINUX"
    abs_path = os.path.abspath(".")
    bin_path = os.path.join(abs_path, "bin")
    VERSION = getVersion("LD_LIBRARY_PATH=%s ./bin/proclist -v | grep Version" % bin_path)
    LIB_FILES = ["lib/*.so", "lib/*.jar"]
    EXE_FILES = ["bin/proclist", "bin/netool"]
    ZIP_CMD = "tar cvzf"
    ZIP_PATH = "%s/linux/" % SERVER_PATH
    ZIP_FILE = "srl_linux_%s.tar.gz" % VERSION.replace('.', '_')
    ZIP_CUR = "srl_current.tar.gz"
else:
    PLATFORM = "MAC OS X"
    abs_path = os.path.abspath(".")
    bin_path = os.path.join(abs_path, "bin")
    VERSION = getVersion("DYLD_LIBRARY_PATH=%s ./bin/proclist -v | grep Version" % bin_path)    
    LIB_FILES = ["bin/*.dylib", "bin/*.jar", "bin/*.jnilib"]
    EXE_FILES = ["bin/proclist", "bin/netool"]
    ZIP_CMD = "tar cvzf"
    ZIP_PATH = "%s/osx/" % SERVER_PATH
    ZIP_FILE = "srl_osx_%s.tar.gz" % VERSION.replace('.', '_')
    ZIP_CUR = "srl_current.tar.gz"    

def pushRelease():
    display("CREATING %s SRL RELEASE:\n" % PLATFORM)
    commands.getoutput("rm -rf %s" % RELEASE_DIR)
    commands.getoutput("mkdir -p %s/srl/bin" % RELEASE_DIR)
    commands.getoutput("mkdir -p %s/srl/lib" % RELEASE_DIR)
    display("copying lib files:")
    for file in LIB_FILES:
        os.system("cp -v %s %s/srl/lib/" % (file, RELEASE_DIR))

    display("copying exe files:")
    for file in EXE_FILES:
        os.system("cp -v %s %s/srl/bin/" % (file, RELEASE_DIR))

    os.system("cp README %s/srl/" % (RELEASE_DIR))
    os.chdir(RELEASE_DIR)
    display("downloading CHANGELOG:")
    os.system("wget http://srl.starnes.de/wiki/ChangeLog?format=txt -O srl/CHANGELOG")
    display("zipping files:")
    os.system("%s %s srl" % (ZIP_CMD, ZIP_FILE))
    display("copying release to srl.starnes.de")
    os.system("scp %s ians@srl.starnes.de:%s" % (ZIP_FILE, ZIP_PATH))
    os.system("ssh ians@srl.starnes.de 'ln -snf %s/%s %s/%s'" % (ZIP_PATH, ZIP_FILE, ZIP_PATH, ZIP_CUR))

def makeDocs():
    display("generating srl docs:")
    os.system("mkdir -p docs/srl")
    os.system("doxygen docs/Doxyfile;cp -rf docs/api ../%s/srl/srl_api" % (RELEASE_DIR) )
    display("generating jsrl docs:")
    os.system("javadoc -d docs/jsrl -sourcepath java/src -subpackages jsrl")
    os.system("cp -rf docs/jsrl ../%s/srl/" % (RELEASE_DIR) )

def pushDocs():
    display("moving docs to srl.starnes.de:")
    os.system("cd %s/srl;zip -r srl_api.zip srl_api jsrl_api" % RELEASE_DIR)
    os.system("cd %s/srl;scp srl_api.zip srl.starnes.de:/var/www/srl/pub/" % RELEASE_DIR)
    os.system('ssh ians@srl.starnes.de "cd /var/www/srl/pub;unzip srl_api.zip"')

def sourceRelease():
    display("building source distribution:")
    os.system("rm -rf %s/srl mkdir -p %s/srl" % (RELEASE_DIR, RELEASE_DIR))
    exclude = ["release/", ".svn/", "classes/", "dummy_classes/", "docs/", "build/", "Debug/", "bin/", "lib/", "*.dylib", "*.jar", "*.jnilib"]
    excludes = ['--exclude "%s"' % e for e in exclude]
    cmd = 'rsync -r --delete --progress %s . %s/srl/' % (" ".join(excludes), RELEASE_DIR)
    print cmd
    os.system(cmd)
    ZIP_PATH = "/var/www/srl/pub/release/source/"
    ZIP_FILE = "srl_src_%s.zip" % VERSION.replace('.', '_')
    ZIP_CUR = "srl_src_current.zip"
    os.system('cd %s;zip -r %s srl/*' % (RELEASE_DIR, ZIP_FILE))
    os.system("scp %s/%s ians@srl.starnes.de:%s" % (RELEASE_DIR, ZIP_FILE, ZIP_PATH))
    os.system("ssh ians@srl.starnes.de 'ln -snf %s/%s %s/%s'" % (ZIP_PATH, ZIP_FILE, ZIP_PATH, ZIP_CUR))    

pushRelease()
#makeDocs()
#pushDocs()
#sourceRelease()

