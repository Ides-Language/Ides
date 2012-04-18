
if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/semparse-library)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/semparse-library)
	if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/semparse-yaml-serialization)
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/semparse-yaml-serialization)
	endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/semparse-yaml-serialization)
endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/semparse-library)

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ext-autolink)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext-autolink/include)
endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ext-autolink)
