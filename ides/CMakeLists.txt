project(ides)

# Now set the header and library paths:
include_directories( ${LLVM_INCLUDE_DIR} )
link_directories( ${LLVM_LIBRARY_DIR} )
set(CMAKE_CXX_FLAGS ${LLVM_CFLAGS})

# Let's suppose we want to build a JIT compiler with support for
# binary code (no interpreter):

add_executable(ides
	main.cpp
	)

# Finally, we link the LLVM libraries to our executable:
target_link_libraries(ides ${LLVM_MODULE_LIBS})


