#-------------------------
# General configuration
#------------------------

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
    include(${CMAKE_CURRENT_LIST_DIR}/envs/msvc.cmake)
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    include(${CMAKE_CURRENT_LIST_DIR}/envs/clang.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/envs/common.cmake)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    include(${CMAKE_CURRENT_LIST_DIR}/envs/gcc.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/envs/common.cmake)
endif()
