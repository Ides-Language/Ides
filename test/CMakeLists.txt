
find_program(LLVM_LLI_EXECUTABLE NAMES lli DOC "lli executable")
find_program(IDESC_EXECUTABLE NAMES idesc DOC "idesc executable")

file(GLOB_RECURSE TESTFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.ides)

foreach (TESTFILE ${TESTFILES})
	message(STATUS "Adding test for ${TESTFILE}")
	add_test(NAME ${TESTFILE} COMMAND /usr/bin/env python 
		"${CMAKE_CURRENT_SOURCE_DIR}/Runner.py" 
		"${IDESC_EXECUTABLE}"
		"${LLVM_LLI_EXECUTABLE}"
		"${CMAKE_CURRENT_SOURCE_DIR}/${TESTFILE}")
endforeach()

