cmake_minimum_required(VERSION 2.8.11)

project(ShinyNinja)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0 -fno-operator-names")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -O0 -fno-operator-names")
endif()

if (MSVC)
    set (CMAKE_PREFIX_PATH "E:/Qt/5.3/msvc2013/")
endif(MSVC)

find_package(Qt5Widgets REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)
