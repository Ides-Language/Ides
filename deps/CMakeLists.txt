
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake"
	)

find_package(Boost COMPONENTS system filesystem program_options regex REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/llvm)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/llvm)
	include_directories(${LLVM_INCLUDE_DIRS})
	link_directories(${LLVM_LIBRARY_DIRS})
	llvm_map_components_to_libraries(LLVM_MODULE_LIBS jit native)
else(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/llvm)
    set(LLVM_LFLAGS all)
    find_package(LLVM REQUIRED)
    set(CMAKE_CXX_FLAGS ${LLVM_CFLAGS})
    include_directories(${LLVM_INCLUDE_DIR})
endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/llvm)

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp)
	set(YAMLCPP_LIBRARY yaml-cpp)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/include)
else(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp)
    find_package(YamlCpp REQUIRED)
    include_directories(${YAMLCPP_INCLUDE_DIR})
endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp)

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/semparse-library)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/semparse-library)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/semparse-library/include)
	
	if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/semparse-yaml-serialization)
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/semparse-yaml-serialization)
		include_directories(${CMAKE_CURRENT_SOURCE_DIR}/semparse-yaml-serialization/include)
	endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/semparse-yaml-serialization)
endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/semparse-library)

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ext-autolink)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext-autolink/include)
endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ext-autolink)
