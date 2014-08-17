file(GLOB_RECURSE testSources *.cpp *.hpp)

include_directories(
	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Tests ${testSources})

target_link_libraries(Tests ShinyNinja)