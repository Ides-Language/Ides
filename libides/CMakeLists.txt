
add_definitions(-DLIBIDES_EXPORTS)

file(GLOB HEADER_MODULES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/include/ides" "${CMAKE_CURRENT_SOURCE_DIR}/include/ides/*")
file(GLOB SRC_MODULES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/src/*")

foreach (DIR ${HEADER_MODULES})
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/ides/${DIR}/")
		file(GLOB_RECURSE MODULE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/ides/${DIR}/*.h")

		set(LIBIDES_HEADERS ${LIBIDES_HEADERS} ${MODULE_HEADERS})
		source_group("${DIR}" FILES ${MODULE_HEADERS})
	else()
		set(HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/ides/${DIR}")
		set(LIBIDES_HEADERS ${LIBIDES_HEADERS} ${HEADER})
		source_group("" FILES ${HEADER})
	endif()
endforeach(DIR ${INCLUDE_MODULES})

foreach (DIR ${SRC_MODULES})
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${DIR}/")
		file(GLOB_RECURSE MODULE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/${DIR}/*.cpp")

		set(LIBIDES_SOURCES ${LIBIDES_SOURCES} ${MODULE_SOURCES})
		source_group("${DIR}" FILES ${MODULE_SOURCES})
	else()
		set(SRCFILE "${CMAKE_CURRENT_SOURCE_DIR}/src/${DIR}")
		set(LIBIDES_SOURCES ${LIBIDES_SOURCES} ${SRCFILE})
		source_group("" FILES ${HEADER})
	endif()
endforeach(DIR ${SRC_MODULES})

find_package(BISON)
find_package(FLEX)

FLEX_TARGET(IdesScanner src/lexer.l  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
BISON_TARGET(IdesParser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
ADD_FLEX_BISON_DEPENDENCY(IdesScanner IdesParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

source_group("Generated" FILES ${FLEX_IdesScanner_OUTPUTS} ${BISON_IdesParser_OUTPUTS})

add_library(ides STATIC
    ${LIBIDES_SOURCES}
    ${LIBIDES_HEADERS}
    ${FLEX_IdesScanner_OUTPUTS}
    ${BISON_IdesParser_OUTPUTS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.l
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y
	)

