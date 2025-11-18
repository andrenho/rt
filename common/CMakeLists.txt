#-------------------------
# General configuration
#------------------------

cmake_minimum_required(VERSION 3.25)
project(topdown
        DESCRIPTION "Create maps for rt"
        LANGUAGES C CXX)

include(../cmake/common.cmake)

#-------------------------
# Main build
#------------------------

# library

add_library(rt-common STATIC
        geometry/point.hh
        geometry/shapes.hh
        geometry/shapes.cc)
target_include_directories(rt-common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})