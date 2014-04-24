Ides Language
=============

[![Build Status](https://travis-ci.org/Ides-Language/Ides.png)](https://travis-ci.org/Ides-Language/Ides)


Build Instructions
==================

At the time of this writing, building the Ides compiler is only supported on Apple OS X and Ubuntu.

1. Install prerequisites:
    * Clang and LLVM version 3.3
    * CMake
    * Flex and Bison
    * Boost 1.48 or later

2. Run CMake to generate the build system:
    cmake .

3. Build the compiler:
    make && make install

You should now be able to compile Ides packages. See the help screen for usage information:
    idesc --help

License
=======

The Ides compiler and standard library is distributed under the terms of the MIT license. See LICENSE.md for details.
