# Locate libclangBasic
#
# This module defines
#  CLANG_FOUND, if false, do not try to link to clang
#  CLANG_LIBRARY, where to find clang
#  CLANG_INCLUDE_DIR, where to find yaml.h
#
# By default, the dynamic libraries of clang will be found. To find the static ones instead,
# you must set the CLANG_STATIC_LIBRARY variable to TRUE before calling find_package(YamlCpp ...).
#
# If clang is not installed in a standard path, you can use the CLANG_DIR CMake variable
# to tell CMake where clang is.

# attempt to find static library first if this is set

# find the clang include directory
find_path(CLANG_INCLUDE_DIR clang/Basic/LLVM.h
          PATH_SUFFIXES include
          PATHS
          ~/Library/Frameworks/clang/include/
          /Library/Frameworks/clang/include/
          /usr/local/include/
          /usr/include/
          /sw/clang/         # Fink
          /opt/local/clang/  # DarwinPorts
          /opt/csw/clang/    # Blastwave
          /opt/clang/
	  ${IDES_ROOT}/deps/llvm/tools/clang/include
          ${CLANG_DIR}/include/)

# find the clang library
find_library(CLANG_LIBRARY
             NAMES libclangBasic.a clangBasic
             PATH_SUFFIXES lib64 lib
             PATHS ~/Library/Frameworks
                    /Library/Frameworks
                    /usr/local
                    /usr
                    /sw
                    /opt/local
                    /opt/csw
                    /opt
	            ${IDES_ROOT}/deps/llvm/build/lib
                    ${CLANG_DIR}/lib)

# handle the QUIETLY and REQUIRED arguments and set CLANG_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CLANG DEFAULT_MSG CLANG_INCLUDE_DIR CLANG_LIBRARY)
mark_as_advanced(CLANG_INCLUDE_DIR CLANG_LIBRARY)

