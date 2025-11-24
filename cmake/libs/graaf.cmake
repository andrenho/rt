set(SKIP_TESTS ON)
set(SKIP_BENCHMARKS ON)
set(SKIP_EXAMPLES ON)
FetchContent_Declare(graaf
        GIT_REPOSITORY https://github.com/bobluppes/graaf.git
        GIT_TAG main
)
FetchContent_MakeAvailable(graaf)
