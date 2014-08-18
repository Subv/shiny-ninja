file(GLOB_RECURSE DesktopSources *.cpp *.hpp)

file(GLOB_RECURSE UI_FILES *.ui)
qt5_wrap_ui(UI_HEADERS ${UI_FILES})

include_directories(
	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR})
	
add_executable(EmulatorDesktop 
    ${DesktopSources}
    ${UI_HEADERS})

target_link_libraries(EmulatorDesktop ShinyNinja)

qt5_use_modules(EmulatorDesktop Widgets)
get_target_property(QtCore_location Qt5::Core LOCATION)
get_target_property(QtWidgets_location Qt5::Widgets LOCATION)

if( UNIX )
    install(TARGETS EmulatorDesktop DESTINATION bin)
elseif( WIN32 )
    install(TARGETS EmulatorDesktop DESTINATION "${CMAKE_INSTALL_PREFIX}")
    add_custom_command(
        TARGET EmulatorDesktop
        COMMAND ${CMAKE_COMMAND} -E copy ${QtWidgets_location} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
    )
    add_custom_command(
        TARGET EmulatorDesktop
        COMMAND ${CMAKE_COMMAND} -E copy ${QtCore_location} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
    )
endif()