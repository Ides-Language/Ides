cmake_minimum_required(VERSION 2.6)
project(ides)

set(IDES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_PROJECT_NAME Ides)
set(IDES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

find_package(Boost COMPONENTS system filesystem program_options regex REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
if (MSVC)
	unset(Boost_LIBRARY_DIRS)
endif(MSVC)

set(LLVM_DIR "${CMAKE_SOURCE_DIR}/deps/llvm/cmake/modules")
find_package(LLVM REQUIRED)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
if(NOT DEFINED LLVM_MODULE_LIBS)
	llvm_map_components_to_libraries(LLVM_MODULE_LIBS all)
endif(NOT DEFINED LLVM_MODULE_LIBS)

set(CMAKE_CXX_FLAGS ${LLVM_CFLAGS})

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/yaml-cpp)
    message(STATUS "Found yaml-cpp in deps.")
	add_subdirectory(${CMAKE_SOURCE_DIR}/deps/yaml-cpp)
	set(YAMLCPP_LIBRARY yaml-cpp)
	include_directories(${CMAKE_SOURCE_DIR}/deps/yaml-cpp/include)
else(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/yaml-cpp)
    find_package(YamlCpp REQUIRED)
    message(STATUS "Found yaml-cpp in CMake modules.")
    include_directories(${YAMLCPP_INCLUDE_DIR})
endif(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/yaml-cpp)

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/ext-autolink)
    message(STATUS "Found ext-autolink in deps.")
    include_directories("${CMAKE_SOURCE_DIR}/deps/ext-autolink/include")
endif(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/ext-autolink)

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/semparse-library)
    message(STATUS "Found semparse-library in deps.")
	add_subdirectory(${CMAKE_SOURCE_DIR}/deps/semparse-library)
	include_directories(${CMAKE_SOURCE_DIR}/deps/semparse-library/include)
endif(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/semparse-library)

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/semparse-yaml-serialization)
    message(STATUS "Found semparse-yaml-serialization in deps.")
    add_subdirectory(${CMAKE_SOURCE_DIR}/deps/semparse-yaml-serialization)
    include_directories(${CMAKE_SOURCE_DIR}/deps/semparse-yaml-serialization/include)
endif(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/semparse-yaml-serialization)

include_directories( 
	"libides/include"
	)

add_subdirectory(libides)
add_subdirectory(ides)

find_package(Doxygen)
if(DOXYGEN_FOUND)
	add_custom_target(docs
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating API documentation" VERBATIM
		)
endif(DOXYGEN_FOUND)

