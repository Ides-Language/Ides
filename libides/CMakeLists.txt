
include_directories(semparse)

add_library(ides STATIC
	# Parsing
	semparse/parse_input.cpp
	# Util
	src/Util/SymbolTable.cpp

	# Types

	# AST

	)

