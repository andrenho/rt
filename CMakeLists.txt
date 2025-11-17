#-------------------------
# General configuration
#------------------------

cmake_minimum_required(VERSION 3.25)
project(rt
        VERSION 0.0.2
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
    include(cmake/msvc.cmake)
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    include(cmake/clang.cmake)
    include(cmake/common.cmake)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    include(cmake/gcc.cmake)
    include(cmake/common.cmake)
endif()

#-------------------------
# Build submodules
#------------------------

add_subdirectory(topdown)
add_subdirectory(map)
