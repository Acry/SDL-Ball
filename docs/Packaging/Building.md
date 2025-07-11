# Building the Package

One does not need to change the Makefile to build the package, but it is recommended to set the `PREFIX` and `DATADIR`
variables to ensure the files are installed in the correct locations.

`make -j33 PREFIX=/usr DATADIR=/games/sdl-ball-themes/`

If you want to switch the compiler, you can set the `CXX` variable. For example, to use `g++` instead of the default
compiler, which is currently clang:

`make CXX=g++`
