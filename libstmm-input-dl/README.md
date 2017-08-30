stmm-input-dl                                                      {#mainpage}
=============

stmm-input-dl is a linux specific library for C++ (C++14) that uses the dlopen
mechanism to load other libraries as plugins (C++14). Specifically it defines
a device manager that loads child plugin device managers at runtime.

Currently the library should only be compiled with g++. The plug-ins, to be
loaded correctly, have to be compiled with the g++-only "-Wl,-E" linker option.

The stmm-input-dl library itself probably should not be statically linked
to other stmm-input related libraries.



Warning
-------
The API of this library isn't stable yet.
