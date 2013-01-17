
file(GLOB_RECURSE TESTFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.ides)

foreach (TESTFILE ${TESTFILES})
	message(STATUS "Adding test for ${TESTFILE}")
	add_test(NAME ${TESTFILE} COMMAND /usr/bin/env python 
		"${CMAKE_CURRENT_SOURCE_DIR}/Runner.py" 
		"${CMAKE_BINARY_DIR}/idesc/idesc"
		"${CMAKE_CURRENT_SOURCE_DIR}/${TESTFILE}")
endforeach()

