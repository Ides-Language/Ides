cmake_minimum_required(VERSION 2.8)
project(ides)

include(CTest)

set(IDES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_PROJECT_NAME Ides)
set(IDES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(CMAKE_PREFIX_PATH ${CMAKE_PROGRAM_PATH} "${CMAKE_SOURCE_DIR}/deps" "${CMAKE_BINARY_DIR}")

set(Boost_USE_STATIC_LIBS        ON)
find_package(Boost 1.48.0 COMPONENTS system filesystem program_options regex serialization REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
if (MSVC)
	unset(Boost_LIBRARY_DIRS)
endif(MSVC)

set(LLVM_REQUIRED_LIBS all)

find_package(LLVM REQUIRED)
message(STATUS "LLVM Include: ${LLVM_INCLUDE_DIRS}")
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
if(NOT DEFINED LLVM_MODULE_LIBS)
	llvm_map_components_to_libraries(LLVM_MODULE_LIBS all)
endif(NOT DEFINED LLVM_MODULE_LIBS)
message(STATUS "LLVM Libraries: ${LLVM_MODULE_LIBS}")

set(CMAKE_CXX_FLAGS ${LLVM_CFLAGS})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	# using Clang
	message(STATUS "Using clang.")
	add_definitions("-Wno-c++11-extensions -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# using GCC
	add_definitions("-std=c++0x")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	# using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# using Visual Studio C++
endif()

include_directories(${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/deps/include")
link_directories("${CMAKE_SOURCE_DIR}/deps/lib")

add_subdirectory(ides)
add_subdirectory(idesc)
#add_subdirectory(ides-headergen)
add_subdirectory(test)

find_package(Doxygen)

if(DOXYGEN_FOUND)
	add_custom_target(docs
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating API documentation" VERBATIM
		)
endif(DOXYGEN_FOUND)

