cmake_minimum_required(VERSION 2.6)
project(ides)

set(IDES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_PROJECT_NAME Ides)
set(IDES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	"${CMAKE_CURRENT_SOURCE_DIR}/deps/cmake"
	)

find_package(Boost COMPONENTS system filesystem program_options regex REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

if(IS_DIRECTORY deps/llvm)
	add_subdirectory(deps/llvm)
	include_directories(${LLVM_INCLUDE_DIRS})
	link_directories(${LLVM_LIBRARY_DIRS})
	llvm_map_components_to_libraries(LLVM_MODULE_LIBS jit native)
else(IS_DIRECTORY deps/llvm)
    set(LLVM_LFLAGS all)
    find_package(LLVM REQUIRED)
    set(CMAKE_CXX_FLAGS ${LLVM_CFLAGS})
    include_directories(${LLVM_INCLUDE_DIR})
endif(IS_DIRECTORY deps/llvm)

if(IS_DIRECTORY deps/yaml-cpp)
	add_subdirectory(deps/yaml-cpp)
	set(YAMLCPP_LIBRARY yaml-cpp)
	include_directories(deps/yaml-cpp/include)
else(IS_DIRECTORY deps/yaml-cpp)
    find_package(YamlCpp REQUIRED)
    include_directories(${YAMLCPP_INCLUDE_DIR})
endif(IS_DIRECTORY deps/yaml-cpp)

if(IS_DIRECTORY deps/semparse-library)
	add_subdirectory(deps/semparse-library)
	include_directories(deps/semparse-library/include)
	
	if(IS_DIRECTORY deps/semparse-yaml-serialization)
		add_subdirectory(deps/semparse-yaml-serialization)
		include_directories(deps/semparse-yaml-serialization/include)
	endif(IS_DIRECTORY deps/semparse-yaml-serialization)
endif(IS_DIRECTORY deps/semparse-library)

if(IS_DIRECTORY deps/ext-autolink)
	include_directories(deps/ext-autolink/include)
endif(IS_DIRECTORY deps/ext-autolink)


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

