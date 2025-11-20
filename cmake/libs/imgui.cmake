include(${CMAKE_CURRENT_LIST_DIR}/raylib.cmake)

set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL 2 REQUIRED)

FetchContent_Populate(imgui
        URL https://github.com/ocornut/imgui/archive/docking.zip
        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/imgui
)

FetchContent_Populate(
        rlimgui
        GIT_REPOSITORY "https://github.com/raylib-extras/rlImGui.git"
        GIT_TAG "main"
        GIT_PROGRESS TRUE
)

add_library(imgui_raylib STATIC
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_opengl2.cpp
        rlimgui-src/rlImGui.cpp
)

target_include_directories(imgui_raylib
        PUBLIC SYSTEM
        /usr/local/include
        ${CMAKE_CURRENT_BINARY_DIR}/imgui
        ${CMAKE_CURRENT_BINARY_DIR}/imgui/backends
        ${CMAKE_CURRENT_BINARY_DIR}/rlimgui-src
)

target_link_libraries(imgui_raylib PUBLIC ${OPENGL_LIBRARIES} raylib)

