#-------------------------
# General configuration
#------------------------

cmake_minimum_required(VERSION 3.25)
project(rt-city
        DESCRIPTION "City generation lib for rt"
        LANGUAGES C CXX)

include(../cmake/common.cmake)

#-------------------------
# Libraries
#------------------------

include(../common/include-common-lib.cmake)

#-------------------------
# Main build
#------------------------

# library

add_library(rt-city STATIC
        city/city.hh
        city/city.cc
)
target_link_libraries(${PROJECT_NAME} PUBLIC rt-common)

# test executable

add_executable(${PROJECT_NAME}-test EXCLUDE_FROM_ALL test/main.cc)
target_include_directories(${PROJECT_NAME}-test PRIVATE ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}-test PRIVATE ${PROJECT_NAME} raylib imgui_raylib)

# special targets

set(FINAL_TARGETS ${PROJECT_NAME}-test)
set(BASE_DIR ..)
include(../cmake/special-targets.cmake)
