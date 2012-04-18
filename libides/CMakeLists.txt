
include_directories(include/ides/semparse)

add_library(ides STATIC
	# Parsing
	src/semparse/parse_input.cpp
	# Util
	src/Util/SymbolTable.cpp

	# Types

	# AST

	)

