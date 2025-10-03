cmake_minimum_required(VERSION 3.5)
project(STG_GAME LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_DEMO OFF)
set(CMAKE_DEBUG_POSTFIX d)
message("Directory is: ${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory("deps/XPFW")

file(GLOB src src/*.cpp)
add_executable(STG ${src})
target_include_directories(STG PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(STG XPFW)

set_target_properties(STG PROPERTIES DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/res/)

option(BUILD_TESTS ON)

#if(BUILD_TESTS)
add_subdirectory("test/")
#endif()