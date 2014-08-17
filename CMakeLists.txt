project(ShinyNinja)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0")

add_subdirectory(src)
add_subdirectory(tests)