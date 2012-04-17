cmake_minimum_required(VERSION 2.6)
project(ides)

set(CMAKE_PROJECT_NAME Ides)
set(IDES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	"${CMAKE_CURRENT_SOURCE_DIR}/deps/cmake"
	)
message(STATUS "Searching in ${CMAKE_MODULE_PATH}")

set(LLVM_LFLAGS all)

add_subdirectory(deps)

find_package(LLVM REQUIRED)
find_package(YamlCpp REQUIRED)
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)

include_directories( 
	${Boost_INCLUDE_DIRS}
	${LLVM_INCLUDE_DIR} 
	${YAMLCPP_INCLUDE_DIR} 
	"${CMAKE_CURRENT_SOURCE_DIR}/libides/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/deps/semparse-library/include"
	)

link_directories( ${LLVM_LIBRARY_DIR} )
set(CMAKE_CXX_FLAGS ${LLVM_CFLAGS})

add_subdirectory(libides)
add_subdirectory(ides)


