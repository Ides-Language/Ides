cmake_minimum_required(VERSION 2.6)

set(CMAKE_PROJECT_NAME Ides)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	"${CMAKE_CURRENT_SOURCE_DIR}/deps/cmake"
	)
message(STATUS "Searching in ${CMAKE_MODULE_PATH}")

set(LLVM_LFLAGS all)

find_package(LLVM REQUIRED)
find_package(YamlCpp REQUIRED)

add_subdirectory(ides-util)
add_subdirectory(ides-parsing)
add_subdirectory(ides-codegen)

add_subdirectory(ides)


