
add_definitions(-DLIBIDES_EXPORTS)

file(GLOB MODULES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*")

foreach (DIR ${MODULES})
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/")
		file(GLOB_RECURSE MODULE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/*.h")
		file(GLOB_RECURSE MODULE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/*.cpp")

		set(LIBIDES_HEADERS ${LIBIDES_HEADERS} ${MODULE_HEADERS})
		set(LIBIDES_SOURCES ${LIBIDES_SOURCES} ${MODULE_SOURCES})

		source_group("${DIR}" FILES ${MODULE_HEADERS})
		source_group("${DIR}" FILES ${MODULE_SOURCES})

		INSTALL(FILES ${MODULE_HEADERS} DESTINATION "include/ides/${DIR}")
	else()
		set(F "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}")

		source_group("" FILES ${F})

		get_filename_component(EXTENSION ${F} EXT)
		if (${EXTENSION} STREQUAL ".h")
			set(LIBIDES_HEADERS ${LIBIDES_HEADERS} ${F})
			INSTALL(FILES ${F} DESTINATION "include/ides")
		else()
			set(LIBIDES_SOURCES ${LIBIDES_SOURCES} ${F})

		endif()
	endif()
endforeach(DIR ${MODULES})

find_package(BISON)
find_package(FLEX)

FLEX_TARGET(IdesScanner lexer.l  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
BISON_TARGET(IdesParser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
ADD_FLEX_BISON_DEPENDENCY(IdesScanner IdesParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

source_group("Generated" FILES ${FLEX_IdesScanner_OUTPUTS} ${BISON_IdesParser_OUTPUTS})

add_library(ides STATIC
	${LIBIDES_SOURCES}
	${LIBIDES_HEADERS}
	${FLEX_IdesScanner_OUTPUTS}
	${BISON_IdesParser_OUTPUTS}
	${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
	${CMAKE_CURRENT_SOURCE_DIR}/parser.y
	)

target_link_libraries(ides
	${Boost_LIBRARIES}
	${LLVM_MODULE_LIBS}
	clangBasic
	clangFrontend
	clangEdit
	clangLex
	xerces-c
	)

INSTALL(TARGETS ides
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

