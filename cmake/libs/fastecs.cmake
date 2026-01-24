FetchContent_Declare(fastecs
        GIT_REPOSITORY https://github.com/andrenho/fast-ecs
        GIT_TAG master
)
FetchContent_MakeAvailable(fastecs)

add_library(fastecs INTERFACE)
target_include_directories(fastecs SYSTEM INTERFACE
        ${fastecs_SOURCE_DIR}
)
