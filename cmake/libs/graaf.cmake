FetchContent_Declare(
        fmtx
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        9.1.0
)
FetchContent_MakeAvailable(fmtx)

add_library(fmtx INTERFACE)
target_include_directories(fmtx SYSTEM INTERFACE
        ${fmtx_SOURCE_DIR}
)

set(SKIP_TESTS ON)
set(SKIP_BENCHMARKS ON)
set(SKIP_EXAMPLES ON)

FetchContent_Declare(graaf
        GIT_REPOSITORY https://github.com/bobluppes/graaf.git
        GIT_TAG main
)
FetchContent_MakeAvailable(graaf)

add_library(graaf INTERFACE)
target_include_directories(graaf SYSTEM INTERFACE
        ${graaf_SOURCE_DIR}/include
)
