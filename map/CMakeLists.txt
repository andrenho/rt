#-------------------------
# General configuration
#------------------------

cmake_minimum_required(VERSION 3.25)
project(topdown
        VERSION 0.0.2
        DESCRIPTION "Create maps for rt"
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
# Main build
#------------------------

# library

add_library(rt-map STATIC
    lib/map.hh lib/map.cc)

# test executable

add_executable(rt-map-test EXCLUDE_FROM_ALL test/main.cc)
target_include_directories(rt-map-test PRIVATE lib)
target_link_libraries(rt-map-test PRIVATE rt-map)

# special targets

set(FINAL_TARGETS rt-map-test)
set(BASE_DIR ..)
include(../cmake/special-targets.cmake)
