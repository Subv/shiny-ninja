file(GLOB_RECURSE sources *.cpp *.hpp)

include_directories(
	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR})
	
if (TESTS)
	add_library(ShinyNinja ${sources})
else(TESTS)
	add_executable(ShinyNinja ${sources})
endif(TESTS)