#-------------------------
# General configuration
#------------------------

cmake_minimum_required(VERSION 3.25)
project(rt-map
        DESCRIPTION "Map generation lib for rt"
        LANGUAGES C CXX)

include(../cmake/common.cmake)

#-------------------------
# Libraries
#------------------------

include(../common/include-common-lib.cmake)

include(../cmake/libs/raylib.cmake)
include(../cmake/libs/imgui.cmake)

#-------------------------
# Main build
#------------------------

# library

add_library(rt-map STATIC
        map/map.hh map/map.cc
)
target_link_libraries(rt-map PUBLIC rt-common)

# test executable

add_executable(rt-map-test EXCLUDE_FROM_ALL test/main.cc)
target_include_directories(rt-map-test PRIVATE rt-map)
target_link_libraries(rt-map-test PRIVATE rt-map raylib imgui_raylib)

# special targets

set(FINAL_TARGETS rt-map-test)
set(BASE_DIR ..)
include(../cmake/special-targets.cmake)
