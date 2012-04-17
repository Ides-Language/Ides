
# Let's suppose we want to build a JIT compiler with support for
# binary code (no interpreter):

add_executable(idesc
	main.cpp
	)

# Finally, we link the LLVM libraries to our executable:
target_link_libraries(idesc
	${Boost_LIBRARIES}
	${LLVM_MODULE_LIBS}
	${YAMLCPP_LIBRARY}
	semparse
	ides
	)


