#!/usr/bin/env python
# encoding: utf-8
"""
release.py

Created by Ian Starnes on 2007-08-28.
Copyright (c) 2007 SRL. All rights reserved.
"""

import sys, os, commands, shutil

def display(msg, newline=True, silent=False):
    if silent:
        return
    if newline:
        msg += '\n'
    sys.stdout.write(msg)
    sys.stdout.flush()

class Release(object):
    def __init__(self, path, platform):
        self.path = path
        self.release_path = "%s/release" % self.path
        self.release_bin = "%s/srl/bin" % self.release_path
        self.release_lib = "%s/srl/lib" % self.release_path
        self.release_include = "%s/srl/include" % self.release_path

        self.platform = platform
        self.version_cmd = None
        self.archive_cmd = None
        self.remote_path = None
        self.archive_file = None
        self.archive_current = "srl_current.tar.gz"
        self.lib_files = []
        self.exe_files = []
        self.debug_cflags = ['-Wall', '-W', '-g']   #extra compile flags for debug
        self.release_cflags = ['-O2', '-Wall', '-W']         #extra compile flags for release
        self.build_cmd = "scons"
        
    def getVersion(self):
        v = commands.getoutput(self.version_cmd)
        v = v[v.find('.')-1:].strip()
        return v
    
    def copyFiles(self, files, dst_path):
        for f in files:
            path, fl = os.path.split(f)
            display("\t\tcopying %s: " % f, False)
            if not os.path.exists(f):
                display("NO SUCH FILE")
                sys.exit(1)
                            
            dst_file = os.path.join(dst_path, fl)
            shutil.copyfile(f, dst_file)
            if not os.path.exists(dst_file):
                display("FAILED")
            else:
                display("done")        
    
    def copyIncludes(self, childpath):
        os.makedirs(os.path.join(self.release_include, childpath))
        path = os.path.join("cpp", "include", childpath)
        files = os.listdir(path)
        for f in files:
            if f.startswith("."):
                continue
            fpath = os.path.join(path, f)
            if os.path.isdir(fpath):
                self.copyIncludes(os.path.join(childpath, f))
            elif f.endswith(".h"):
                display("\t\tcopying %s/%s" % (childpath, f))
                shutil.copyfile(fpath, os.path.join(self.release_include, childpath, f))
            
    
    def makeSetupScript(self, path=""):
        setup_file = os.path.join(path, "setup.sh")
        f = open(setup_file, 'w')
        f.write(self.setup_script)
        f.close()
        import stat
        os.chmod(setup_file, stat.S_IRWXG | stat.S_IRWXU)        

    def makeRelease(self):
        display("== creating binary release for %s ==" % self.platform)
        
        display("\tremoving old files")
        shutil.rmtree("%s" % self.release_path)
        
        os.makedirs(self.release_lib)        
        display("\tcopying lib files")
        self.copyFiles(self.lib_files, self.release_lib)
        
        os.makedirs(self.release_bin)
        display("\tcopying binary files")
        self.copyFiles(self.bin_files, self.release_bin)
        
        os.makedirs(self.release_include)
        display("\tcopying include files")
        self.copyIncludes("srl")
    
        shutil.copyfile("README", os.path.join(self.release_path, "srl", "README"))
        self.makeSetupScript(os.path.join(self.release_path, "srl"))
        
        display("\tcreating release file")
        curpath = os.getcwd()
        os.chdir(self.release_path)
        os.system("%s %s srl" % (self.archive_cmd, self.archive_file))
        if not os.path.exists(self.archive_file):
            display("FAILED TO CREATE RELEASE")
            return
        display("release file created")
        display(" ")
        os.chdir(curpath)
        
        
    def postRelease(self):
        display("== posting binary release for %s ==" % self.platform)
        display("\tcopying %s to srl.starnes.de" % self.archive_file)
        os.system("scp %s/%s ians@srl.starnes.de:%s" % (self.release_path, self.archive_file, self.remote_path))
        os.system("ssh ians@srl.starnes.de 'ln -snf %s/%s %s/%s'" % (self.remote_path, self.archive_file, self.remote_path, self.archive_current))
        display("done")
        display(" ")

    def makeDocs(self):
        display("== generating docs ==")
        
    def postDocs(self):
        display("== posting docs ==")
         
    def build(self, debug=False):
        display("== building SRL ==")
        os.system("scons -Q")
        display("done")
        display(" ")
        
    def clean(self):
        display("== Cleaning SRL Files ==")
        os.system("scons -c")
        display("done")
        display(" ")        
        
    def buildJava(self):
        display("== building SRL Java Bindings ==")
        os.system("scons -Q jsrl")
        display("done")
        display(" ")

    def cleanJava(self):
        display("== Cleaning SRL Library Files ==")
        os.system("scons -c jsrl")
        display("done")
        display(" ")
        
    def buildTests(self):
        display("== building SRL Unit Tests ==")
        os.system("scons -Q tests")
        display("done")
        display(" ")
        
    def runTests(self):
        display("== running SRL Tests ==")
        os.system("bin/tests")        
        display("done")
        display(" ")

    def cleanTests(self):
        display("== Cleaning SRL Library Files ==")
        os.system("scons -c tests")
        display("done")
        display(" ")
        
    def buildExamples(self):
        display("== building SRL Examples ==")
        os.system("scons -Q examples")
        display("done")
        display(" ")
        
    def cleanExamples(self):
        display("== Cleaning SRL Example Files ==")
        os.system("scons -c examples")
        display("done")
        display(" ")


class Linux(Release):
    def __init__(self):
        abs_path = os.path.abspath(".")
        Release.__init__(self, abs_path, "linux")
        bin_path = os.path.join(abs_path, "bin")        
        self.version_cmd = "LD_LIBRARY_PATH=%s ./bin/proclist -v | grep Version" % bin_path
        self.archive_cmd = "tar czf"
        self.remote_path = "/var/www/srl/pub/release/linux"
        self.archive_current = "srl_current.tar.gz"
        self.archive_file = "srl_%s_%s.tar.gz" % (self.platform, self.getVersion())
        
        self.lib_files = ["lib/libsrl.so", "lib/libjsrl.so", "lib/jsrl.jar"]
        self.bin_files = ["bin/netool", "bin/proclist"]
        
        self.setup_script = """
echo "SETTING LD_LIBRARY_PATH=$PWD/lib"
LD_LIBRARY_PATH=$PWD/lib
"""

        
class OSX(Release):
    def __init__(self):
        abs_path = os.path.abspath(".")
        Release.__init__(self, abs_path, "osx")
        bin_path = os.path.join(abs_path, "bin")        
        self.version_cmd = "LD_LIBRARY_PATH=%s ./bin/proclist -v | grep Version" % bin_path
        self.archive_cmd = "tar czf"
        self.remote_path = "/var/www/srl/pub/release/linux"
        self.archive_current = "srl_current.tar.gz"
        self.archive_file = "srl_%s_%s.tar.gz" % (self.platform, self.getVersion())

        self.lib_files = ["lib/libsrl.dylib", "lib/libjsrl.jnilib", "lib/jsrl.jar"]
        self.bin_files = ["bin/netool", "bin/proclist"]

        self.setup_script = """
echo "SETTING LD_LIBRARY_PATH=$PWD/lib"
export LD_LIBRARY_PATH=$PWD/lib
echo "SETTING DYLD_LIBRARY_PATH=$PWD/lib"
export DYLD_LIBRARY_PATH=$PWD/lib
"""


class Windows(Release):
    def __init__(self):
        abs_path = os.path.abspath(".")
        Release.__init__(self, abs_path, "win32")
        bin_path = os.path.join(abs_path, "bin")        
        self.version_cmd = "LD_LIBRARY_PATH=%s ./bin/proclist -v | grep Version" % bin_path
        self.archive_cmd = "tar czf"
        self.remote_path = "/var/www/srl/pub/release/linux"
        self.archive_current = "srl_current.tar.gz"
        self.archive_file = "srl_%s_%s.tar.gz" % (self.platform, self.getVersion())

        self.lib_files = ["lib/srl.lib", "lib/jsrl.lib", "lib/jsrl.jar"]
        self.bin_files = ["bin/srl.dll", "bin/jsrl.dll", "bin/netool.exe", "bin/proclist.exe"]
        
        # build command
        plat = platform.system().lower()
        if plat.startswith("cygwin"):
            if not os.path.exists("/usr/bin/vcbuild"):
                # see if we can find vcbuild.exe
                vcpath = commands.getoutput("cygpath $PROGRAMFILES").replace("\n", " ")
                VC = "Microsoft Visual Studio 8/VC/vcpackages/vcbuild.exe"
                vcpath = "%s/%s" % (vcpath, VC)
                if not os.path.exists(vcpath):
                    display("Visual C++ Express could not be found")
                    sys.exit(1)
                commands.getoutput("ln -snf %s /usr/bin/vcbuild" % vcpath.replace(" ", "\\ "))
        self.build_cmd = "vcbuild"
        self.setup_script = ""
        self.debug_cflags = ['-W1', '-GX', '-EHsc', '-D_DEBUG', '/MDd']
        self.release_cflags = ['-O2', '-EHsc', '-DNDEBUG', '/MD']        #extra compile flags for release


import platform
cur = platform.system().lower()
if cur.startswith("darwin"):
    PLATFORM = OSX()
elif cur.startswith("linux"):
    PLATFORM = Linux()
else:
    PLATFORM = Windows()

TARGETS = {'all':"Build everything except docs", 
 'srl':"Build the C++ Libraries", 
 'java':"Build the java bindings", 
 'examples':"Build the examples", 
 'tests':"Build and runs the unit tests", 
 'docs':"Generate the docs (use -p to publish)", 
 'release':"Generate a release file (use -p to publish)"}

def build(opts):
    targets = opts.args
    
    if "all" in targets:
        PLATFORM.build()
        PLATFORM.buildJava()
        PLATFORM.buildExamples()
        PLATFORM.buildTests()


    if "srl" in targets:
        PLATFORM.build()
    if "java" in targets:
        PLATFORM.buildJava()
    if "examples" in targets:
        PLATFORM.buildExamples()
    if "tests" in targets:
        PLATFORM.buildTests()
        PLATFORM.runTests()
    
    if "docs" in targets:
        PLATFORM.makeDocs()
        if opts.post:
            PLATFORM.postDocs()
    if "release" in targets:
        PLATFORM.makeRelease()    
        if opts.post:
            PLATFORM.postRelease()    

def clean(opts):
    targets = opts.args
    
    if "all" in targets:
        PLATFORM.clean()
        PLATFORM.cleanJava()
        PLATFORM.cleanExamples()
        PLATFORM.cleanTests()


    if "srl" in targets:
        PLATFORM.clean()
    if "java" in targets:
        PLATFORM.cleanJava()
    if "examples" in targets:
        PLATFORM.cleanExamples()
    if "tests" in targets:
        PLATFORM.cleanTests()
        PLATFORM.runTests()

def main(opts):
    if opts.clean:
        clean(opts)
    else:
        build(opts)
        
if __name__ == '__main__':
    from optparse import OptionParser
    required_args = 0
    usage = """usage: 
        %prog""" + " [options] target\ntargets:\n%s" % "\n".join(["   %-10s\t%s" % (target, TARGETS[target]) for target in TARGETS])

    description = """
SRL Build Tool
"""
    options = OptionParser(usage=usage, description=description, version="0.1")
    options.add_option("-c", "--clean", action="store_true", help="clean the specified target/s", dest="clean", default=False)
    options.add_option("-d", "--debug", action="store_true", help="build debug mode", dest="debug", default=False)
    options.add_option("-p", "--post", action="store_true", help="post a release", dest="post", default=False)
    options.add_option("-v", "--verbose", action="store_true", help="show verbose output", dest="verbose", default=False)
    # running this actually parses the argument list
    opts, args = options.parse_args()
    # to make thing simple we add any arguments that have not been parsed to our option list
    opts.args = args
    # add the argument count
    opts.argc = len(args)
    # if we did not receive the required amount of arguments show the help and exit
    if opts.argc == 0:
        options.print_help()
        sys.exit(1)
    # call main with our options and parsed arguments
    main(opts)

