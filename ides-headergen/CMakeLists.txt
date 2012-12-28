
add_executable(ides-headergen
	main.cpp
	)

# Finally, we link the LLVM libraries to our executable:
target_link_libraries(ides-headergen
	clang
	clangARCMigrate
	clangAST
	clangAnalysis
	clangBasic
	clangCodeGen
	clangDriver
	clangEdit
	clangFrontend
	clangFrontendTool
	clangLex
	clangParse
	clangRewrite
	clangSema
	clangSerialization
	clangStaticAnalyzerCheckers
	clangStaticAnalyzerCore
	clangStaticAnalyzerFrontend
	clangTooling
	${Boost_LIBRARIES}
	${LLVM_MODULE_LIBS}
	)


INSTALL(TARGETS ides-headergen
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

