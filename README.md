SRL
===

Simple Running Library, is cross platform C++ library that handles most things that should be common across platforms.

This project was started back in 1999 and stopped in 2009.  I still use the library on projects but more often I use it as reference.

This has some great code for cross platform low resource monitoring, networking, XML parsing, etc.

Much better libraries exist now like BOOST.

= INSTALLATION =

You can use SCons or on Unix systems you can use make.


== Make ==
cd cpp
make
sudo make install

== SCons ==
scons 

== MAC OSX ==

=== C++ Support ===
libsrl.dylib needs to go in your /usr/lib directory 
or 
add its parent folder to the DYLD_LIBRARY_PATH environment variable

=== Java Support ===
libjsrl.jnilib  needs to go in your /usr/lib/java directory 
or 
add its parent folder to the -Djava.library.path=parent_folder_path





== GENERAL NOTES ==
Remember to be static_cast things when changing types.  This is important 
for such things as float64 and float32 conversions.


7.22.2009 - added System::Reboot() method (windows, linux, NO OSX)
7.22.2009 - added NetworkInterface setIP methods (linux, OSX, no windows)
