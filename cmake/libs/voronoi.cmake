FetchContent_Declare(jc_voronoi
        GIT_REPOSITORY https://github.com/JCash/voronoi.git
        GIT_TAG dev
)
FetchContent_MakeAvailable(jc_voronoi)

add_library(jc_voronoi INTERFACE)
target_include_directories(jc_voronoi SYSTEM INTERFACE
        ${jc_voronoi_SOURCE_DIR}/src
)