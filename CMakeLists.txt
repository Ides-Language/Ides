cmake_minimum_required(VERSION 2.6)
project(ides)

set(IDES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_PROJECT_NAME Ides)
set(IDES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(deps)

include_directories( 
	"${CMAKE_CURRENT_SOURCE_DIR}/libides/include"
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

