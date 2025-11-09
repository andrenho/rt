#-------------------------
# General configuration
#------------------------

cmake_minimum_required(VERSION 3.25)
project(topdown
        VERSION 0.0.2
        DESCRIPTION "A library for topdown 2d games"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

#-------------------------
# Compiler specific code
#------------------------

if(MSVC)
    include(../cmake/msvc.cmake)
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    include(../cmake/clang.cmake)
    include(../cmake/common.cmake)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    include(../cmake/gcc.cmake)
    include(../cmake/common.cmake)
endif()

#-------------------------
# Libraries
#------------------------

# Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
        raylib
        GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
        GIT_TAG "master"
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

# box2d
find_package(box2d REQUIRED)


#-------------------------
# Main build
#------------------------

# library

add_library(topdown STATIC
        lib/world.hh lib/world.cc lib/vehicleconfig.hh lib/vehicle.hh lib/vehicle.cc)
target_link_libraries(topdown PUBLIC box2d::box2d)

# test executable

add_executable(topdown-test EXCLUDE_FROM_ALL test/main.cc)
target_include_directories(topdown-test PRIVATE lib)
target_link_libraries(topdown-test PRIVATE topdown raylib)

add_executable(topdown-test-simple EXCLUDE_FROM_ALL test/simple.cc)
target_include_directories(topdown-test-simple PRIVATE lib)
target_link_libraries(topdown-test-simple PRIVATE topdown)

# special targets

set(FINAL_TARGETS topdown-test topdown-test-simple)
set(BASE_DIR ..)
include(../cmake/special-targets.cmake)
