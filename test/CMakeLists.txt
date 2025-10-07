# List test
add_executable(list_test list.cpp)
target_include_directories(list_test PUBLIC ${CMAKE_HOME_DIRECTORY}/include/)
set_target_properties(list_test PROPERTIES DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/res/)

if (MSVC)
    target_link_options(list_test PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(list_test PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()

# Script test
add_executable(script_test script.cpp)
target_include_directories(script_test PUBLIC ${CMAKE_HOME_DIRECTORY}/include/)
set_target_properties(script_test PROPERTIES DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/res/)

if (MSVC)
    target_link_options(script_test PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(script_test PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()