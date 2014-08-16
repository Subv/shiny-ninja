file(GLOB_RECURSE sources *.cpp *.hpp)

include_directories(
	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR})
	
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0")

if (TESTS)
	add_library(ShinyNinja ${sources})
else(TESTS)
	add_executable(ShinyNinja ${sources})
endif(TESTS)
