file(GLOB_RECURSE sources *.cpp *.hpp)

include_directories(
	${CMAKE_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR})
	
add_executable(ShinyNinja ${sources})