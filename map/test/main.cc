#include <raylib.h>

#include "rlImGui.h"
#include "imgui.h"

#include "map/map.hh"

bool show_demo_window = true;

static void draw_map(map::Map const& map)
{
}

void draw_ui()
{
    rlImGuiBegin();
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    rlImGuiEnd();
}

int main()
{
    map::Map map;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1600, 900, "rt-map-test");
    SetTargetFPS(60);

    rlImGuiSetup(true);
    Camera2D camera { { 0, 0 }, { 0, 0 }, 0, 1.0f };

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(WHITE);

        BeginMode2D(camera);
        draw_map(map);
        draw_ui();
        EndMode2D();

        EndDrawing();

        if (IsKeyDown(KEY_Q))
            break;
    }

    rlImGuiEnd();
}