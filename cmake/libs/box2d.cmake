include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

set(BOX2D_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        box2d
        GIT_REPOSITORY "https://github.com/erincatto/box2d.git"
        GIT_TAG "main"
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(box2d)

get_target_property(dirs box2d INTERFACE_INCLUDE_DIRECTORIES)
if(dirs)
    target_include_directories(box2d SYSTEM INTERFACE ${dirs})
endif()