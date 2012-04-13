
# Let's suppose we want to build a JIT compiler with support for
# binary code (no interpreter):

add_executable(ides
	main.cpp
	)

# Finally, we link the LLVM libraries to our executable:
target_link_libraries(ides 
	${Boost_LIBRARIES}
	${LLVM_MODULE_LIBS}
	${YAMLCPP_LIBRARY}
	${CLANG_LIBRARY}
	libides
	)


