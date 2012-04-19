
include_directories(include/ides/semparse)

add_definitions(-DLIBIDES_EXPORTS)

set(LIBIDES_SOURCES
    # Parsing
	src/semparse/parse_input.cpp
	# Util
	src/Util/SymbolTable.cpp

	# Types

	# AST
	)

set(LIBIDES_HEADERS
    include/ides/common.h
    include/ides/ides.h
    
    # Parsing
    include/ides/Util/SymbolTable.h
    
    # Types
    include/ides/Types/IdesType.h
    
    # AST
    include/ides/AST/ASTNode.h
    )
add_library(ides STATIC
    ${LIBIDES_SOURCES}
    ${LIBIDES_HEADERS}
	)

