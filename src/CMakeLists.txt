file(GLOB_RECURSE sources 
	Common/*.cpp Common/*.hpp
	CPU/*.cpp CPU/*.hpp
	Decoder/*.cpp Decoder/*.hpp
	GPU/*.cpp GPU/*.hpp
	Interpreter/*.cpp Interpreter/*.hpp
	Memory/*.cpp Memory/*.hpp
    DMA/*.cpp DMA/*.hpp)

include_directories(
	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR})
	
add_library(ShinyNinja ${sources})

add_subdirectory(Platform)
