cmake_minimum_required(VERSION 3.5)
project(STG_GAME LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_DEMO OFF)

if("" STREQUAL "${CMAKE_BUILD_TYPE}")
	message("Configuration not selected, defaulting to debug")
	set(CMAKE_BUILD_TYPE "Debug")
	message("Config is now ${CMAKE_BUILD_TYPE}")
endif()

# Set some variables to disable stuff I don't want to build.
# I only use static libs so I have less files to deal with,
# and also use some debug postfix like d for debug builds
set(CMAKE_DEBUG_POSTFIX "d")

# Replace the "Release" string however was written so
# the CMAKE_DEBUG_POSTFIX variable can be "removed"
string(REGEX REPLACE "([Rr][Ee][Ll][Ee][Aa][Ss][Ee])" "RELEASE" RELEASE_CFG ${CMAKE_BUILD_TYPE})

if("RELEASE" STREQUAL ${RELEASE_CFG})
set(CMAKE_DEBUG_POSTFIX "")
endif()

message("Debug postfix is: ${CMAKE_DEBUG_POSTFIX} for ${CMAKE_BUILD_TYPE}" )

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