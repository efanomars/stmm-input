stmm-input-dl                                                      {#mainpage}
=============

stmm-input-dl is a linux specific library for C++ (C++14) that uses the dlopen
mechanism to load other libraries as plugins (C++14). Specifically it defines
a device manager that loads child plugin device managers at runtime.

Currently the library should only be compiled with g++. The plug-ins, to be
loaded correctly, have to be compiled with the g++-only "-Wl,-E" linker option.

The stmm-input-dl library itself probably should not be statically linked
to other stmm-input related libraries.

The plugins are loaded from the following paths
 - /usr/local/etc/stmm-input-dl
 - /etc/stmm-input-dl

The plugin file names must start with two number characters that define the
rank and must end with .dlp. The name of the plugin is the rest of the string.
If two plugins have the same name the one with lowest rank is loaded.
Example: if two files 77myplugin.dlp and 54myplugin.dlp are present in the
plugin paths, only the latter is loaded.

See the API documentation to find out more about the format of a .dlp file.


Warning
-------
The API of this library isn't stable yet.
