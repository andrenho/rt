FetchContent_Declare(perlin
        GIT_REPOSITORY https://github.com/Reputeless/PerlinNoise
        GIT_TAG master
)
FetchContent_MakeAvailable(perlin)

add_library(perlin INTERFACE)
target_include_directories(perlin SYSTEM INTERFACE
        ${perlin_SOURCE_DIR}
)
