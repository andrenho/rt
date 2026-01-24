#-------------------------
# General configuration
#------------------------

cmake_minimum_required(VERSION 3.25)
project(rt-game
        DESCRIPTION "rt game engine"
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

add_library(rt-game STATIC
        game/game.hh
        game/game.cc
)
target_link_libraries(rt-game PUBLIC rt-common rt-map topdown)

# test executable

add_executable(rt-game-test EXCLUDE_FROM_ALL test/main.cc)
target_include_directories(rt-game-test PRIVATE rt-game)
target_link_libraries(rt-game-test PRIVATE rt-game raylib imgui_raylib)

# special targets

set(FINAL_TARGETS rt-game-test)
set(BASE_DIR ..)
include(../cmake/special-targets.cmake)
