import os,platform,SCons,glob,re
import build

platform = platform.system()
prefix = '/usr/local/bin'
override = '.'
#########################
#   Global Environment  #
#########################
baseEnv=Environment()
if ARGUMENTS.get('debug',0):
        baseEnv.Append(CCFLAGS = "-g")
        platform+="-Debug"
if ARGUMENTS.get('optimized',0):
        baseEnv.Append(CCFLAGS = "-O2")
        platform+="-Optimized"
if ARGUMENTS.get('CC',0):
        baseEnv.Replace(CC = ARGUMENTS.get('CC'))
if ARGUMENTS.get('CXX',0):
        baseEnv.Replace(CXX = ARGUMENTS.get('CXX'))
if ARGUMENTS.get('CCFLAGS',0):
        baseEnv.Append(CCFLAGS = ARGUMENTS.get('CCFLAGS'))
if ARGUMENTS.get('CXXFLAGS',0):
        baseEnv.Append(CXXFLAGS = ARGUMENTS.get('CXXFLAGS'))
if ARGUMENTS.get('variant',0):
        platform = ARGUMENTS.get('Variant')
if ARGUMENTS.get('prefix',0):
        prefix=ARGUMENTS.get('prefix')
if ARGUMENTS.get('override',0):
        override=ARGUMENTS.get('override')
        SConsignFile(os.path.join(override,'.sconsign.dblite'))


print "Current Platform: ", 
print build.PLATFORM.platform

build.PLATFORM.makeSetupScript()


mymode = "release"
mymode = "debug"

env = Environment()

#append the user's additional compile flags
#assume debugcflags and releasecflags are defined
if mymode == 'debug':
   env.Append(CCFLAGS=build.PLATFORM.debug_cflags)
else:
   env.Append(CCFLAGS=build.PLATFORM.release_cflags)

SConscript(['cpp/SConscript', 'radmin/SConscript', 'java/SConscript'], exports="env")

