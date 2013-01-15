
add_executable(ides-headergen
	main.cpp
	)

# Finally, we link the LLVM libraries to our executable:
target_link_libraries(ides-headergen
	clangFrontendTool
	clangFrontend
	clangSerialization
	clangDriver
	clangLex
	clangParse
	clangSema
	clangTooling
	clangAST
	clangAnalysis
	clangEdit
	clangBasic
	clang
	${Boost_LIBRARIES}
	${LLVM_MODULE_LIBS}
	pthread
	dl
	)


INSTALL(TARGETS ides-headergen
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

