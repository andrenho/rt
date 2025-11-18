#-------------------------
# General configuration
#------------------------

cmake_minimum_required(VERSION 3.25)
project(topdown
        DESCRIPTION "A library for topdown 2d games"
        LANGUAGES C CXX)

include(../cmake/common.cmake)

#-------------------------
# Libraries
#------------------------

add_subdirectory(../common)

include(../cmake/libs/box2d.cmake)
include(../cmake/libs/raylib.cmake)

#-------------------------
# Main build
#------------------------

# library

add_library(topdown STATIC
        lib/util.hh
        lib/vehicleconfig.hh
        lib/event.hh
        lib/explosivedef.hh
        lib/category.hh
        lib/sensormodifier.hh
        lib/world.hh                    lib/world.cc
        lib/shapes.hh                   lib/shapes.cc
        lib/object.hh                   lib/object.cc
        lib/static/staticobject.hh      lib/static/staticobject.cc
        lib/static/sensor.hh
        lib/dynamic/dymamicobject.hh    lib/dynamic/dynamicobject.cc
        lib/dynamic/wheel.hh            lib/dynamic/wheel.cc
        lib/dynamic/vehicle.hh          lib/dynamic/vehicle.cc
        lib/dynamic/person.cc           lib/dynamic/person.hh
        lib/dynamic/pushableobject.cc   lib/dynamic/pushableobject.hh
        lib/dynamic/explosive.cc        lib/dynamic/explosive.hh
        lib/dynamic/missile.cc          lib/dynamic/missile.hh
        lib/dynamic/shrapnel.cc         lib/dynamic/shrapnel.hh
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
