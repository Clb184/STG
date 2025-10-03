add_executable(list_test list.cpp  ${CMAKE_HOME_DIRECTORY}/src/List.cpp)
target_include_directories(list_test PUBLIC ${CMAKE_HOME_DIRECTORY}/include/)
set_target_properties(list_test PROPERTIES DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/res/)

if (MSVC)
    target_link_options(list_test PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(list_test PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()