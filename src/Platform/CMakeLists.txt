file(GLOB_RECURSE DesktopSources *.cpp *.hpp)

include_directories(
	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR})
	
add_executable(EmulatorDesktop ${DesktopSources})
target_link_libraries(EmulatorDesktop ShinyNinja)