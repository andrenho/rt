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

include(../common/include-common-lib.cmake)

include(../cmake/libs/box2d.cmake)
include(../cmake/libs/raylib.cmake)

#-------------------------
# Main build
#------------------------

# library

add_library(topdown STATIC
        topdown/vehicleconfig.hh
        topdown/event.hh
        topdown/explosivedef.hh
        topdown/category.hh
        topdown/sensormodifier.hh
        topdown/world.hh topdown/world.cc
        topdown/shapes.hh topdown/shapes.cc
        topdown/object.hh topdown/object.cc
        topdown/static/staticobject.hh topdown/static/staticobject.cc
        topdown/static/sensor.hh
        topdown/dynamic/dymamicobject.hh topdown/dynamic/dynamicobject.cc
        topdown/dynamic/wheel.hh topdown/dynamic/wheel.cc
        topdown/dynamic/vehicle.hh topdown/dynamic/vehicle.cc
        topdown/dynamic/person.cc topdown/dynamic/person.hh
        topdown/dynamic/pushableobject.cc topdown/dynamic/pushableobject.hh
        topdown/dynamic/explosive.cc topdown/dynamic/explosive.hh
        topdown/dynamic/missile.cc topdown/dynamic/missile.hh
        topdown/dynamic/shrapnel.cc topdown/dynamic/shrapnel.hh
)
target_link_libraries(topdown PUBLIC box2d rt-common)

# test executable

add_executable(topdown-test EXCLUDE_FROM_ALL test/main.cc)
target_include_directories(topdown-test PRIVATE topdown)
target_link_libraries(topdown-test PRIVATE topdown raylib)

add_executable(topdown-test-simple EXCLUDE_FROM_ALL test/simple.cc)
target_include_directories(topdown-test-simple PRIVATE topdown)
target_link_libraries(topdown-test-simple PRIVATE topdown)

# special targets

set(FINAL_TARGETS topdown-test topdown-test-simple)
set(BASE_DIR ..)
include(../cmake/special-targets.cmake)
