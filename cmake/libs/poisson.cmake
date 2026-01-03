FetchContent_Declare(poisson
        GIT_REPOSITORY https://github.com/thinks/tph_poisson
        GIT_TAG master
)
FetchContent_MakeAvailable(poisson)

add_library(poisson INTERFACE)
target_include_directories(poisson SYSTEM INTERFACE
        ${poisson_SOURCE_DIR}/include/thinks
)
