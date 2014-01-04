cmake_minimum_required(VERSION 2.8)
project(ides)

include(CTest)

set(IDES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_PROJECT_NAME Ides)
set(IDES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(CMAKE_PREFIX_PATH
	"${CMAKE_SOURCE_DIR}/deps/llvm/cmake/modules"
	"${CMAKE_SOURCE_DIR}/deps"
	"${CMAKE_BINARY_DIR}"
	${CMAKE_PROGRAM_PATH}
	)
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

set(Boost_USE_STATIC_LIBS        ON)
find_package(Boost 1.48.0 COMPONENTS system filesystem program_options regex serialization REQUIRED)

if (MSVC)
	unset(Boost_LIBRARY_DIRS)
endif(MSVC)

set(LLVM_REQUIRED_LIBS engine)
set(YAMLCPP_STATIC_LIBRARY On)
find_package(LLVM REQUIRED)
find_package(YamlCpp REQUIRED)

message(STATUS "LLVM Include: ${LLVM_INCLUDE_DIRS}, Lib: ${LLVM_LIBRARY_DIRS}")
if(NOT DEFINED LLVM_MODULE_LIBS)
	llvm_map_components_to_libraries(LLVM_MODULE_LIBS ${LLVM_REQUIRED_LIBS})
endif(NOT DEFINED LLVM_MODULE_LIBS)
message(STATUS "LLVM Libraries: ${LLVM_MODULE_LIBS}")
message(STATUS "YAML Libraries: ${YAMLCPP_LIBRARY}")

set(CMAKE_CXX_FLAGS ${LLVM_CFLAGS})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	# using Clang
	message(STATUS "Using clang.")
	add_definitions("-Wno-c++11-extensions -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS")

	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
	set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++0x -stdlib=libc++ -g -Wall")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# using GCC
	add_definitions("-std=c++0x")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	# using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# using Visual Studio C++
endif()

include_directories(
	"${CMAKE_SOURCE_DIR}" 
	"${LLVM_INCLUDE_DIRS}" 
	"${Boost_INCLUDE_DIRS}" 
	"${YAMLCPP_INCLUDE_DIR}"
	)
link_directories(
	"${LLVM_LIBRARY_DIRS}" 
	"${Boost_LIBRARY_DIRS}"
	)
add_definitions(${LLVM_DEFINITIONS})

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

