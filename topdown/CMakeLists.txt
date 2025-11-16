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

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# Raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
        raylib
        GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
        GIT_TAG "master"
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(raylib)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_definitions(raylib PRIVATE _DARWIN_C_SOURCE)
endif()

# box2d
set(BOX2D_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        box2d
        GIT_REPOSITORY "https://github.com/erincatto/box2d.git"
        GIT_TAG "main"
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(box2d)

# Remove warnings for headers
function(mark_as_system TARGET)
    get_target_property(dirs ${TARGET} INTERFACE_INCLUDE_DIRECTORIES)
    if(dirs)
        target_include_directories(${TARGET} SYSTEM INTERFACE ${dirs})
    endif()
endfunction()

mark_as_system(raylib)
mark_as_system(box2d)

#-------------------------
# Main build
#------------------------

# library

add_library(topdown STATIC
        lib/world.hh lib/world.cc
        lib/util.hh
        lib/shapes.hh lib/shapes.cc
        lib/vehicleconfig.hh
        lib/object.hh
        lib/dynamic/dymamicobject.hh lib/dynamic/dynamicobject.cc
        lib/dynamic/wheel.hh lib/dynamic/wheel.cc
        lib/dynamic/vehicle.hh lib/dynamic/vehicle.cc
        lib/static/staticobject.hh lib/static/staticobject.cc
        lib/static/sensor.hh
        lib/event.hh
        lib/sensormodifier.hh
        lib/dynamic/person.cc
        lib/dynamic/person.hh
        lib/dynamic/pushableobject.cc
        lib/dynamic/pushableobject.hh
        lib/object.cc
        lib/dynamic/explosive.cc
        lib/dynamic/explosive.hh
        lib/dynamic/missile.cc
        lib/dynamic/missile.hh
        lib/explosivedef.hh
        lib/category.hh
)
target_link_libraries(topdown PUBLIC box2d)

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
