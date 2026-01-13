FetchContent_Declare(quickhull
        GIT_REPOSITORY https://github.com/tomilov/quickhull
        GIT_TAG master
)
FetchContent_Populate(quickhull)

add_library(quickhull INTERFACE)
target_include_directories(quickhull SYSTEM INTERFACE
        ${quickhull_SOURCE_DIR}/include
)
