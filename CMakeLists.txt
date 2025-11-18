#-------------------------
# General configuration
#------------------------

cmake_minimum_required(VERSION 3.25)
project(rt
        VERSION 0.0.2
        LANGUAGES C CXX)

include(cmake/common.cmake)

#-------------------------
# Build submodules
#------------------------

add_subdirectory(topdown)
#add_subdirectory(map)
