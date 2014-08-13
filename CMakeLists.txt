project(ShinyNinja)

add_subdirectory(src)

if (TESTS)
	add_subdirectory(tests)
endif()