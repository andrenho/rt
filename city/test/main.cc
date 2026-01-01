#include <raylib.h>
#include "rlImGui.h"
#include "imgui.h"

#include "city/city.hh"
#include "geometry/shapes.hh"

static city::CityConfig city_config;

static Camera2D camera { { 0, 0 }, { 0, 0 }, 0, 1.0f };
static bool show_demo_window = true;

static void handle_events()
{
}

static void draw()
{
}

static void draw_ui()
{
    rlImGuiBegin();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    rlImGuiEnd();
}

int main()
{
    srand((unsigned int) time(nullptr));
    city_config.seed = rand();

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1600, 900, "rt-city-test");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    while (!WindowShouldClose()) {

        handle_events();

        BeginDrawing();
        ClearBackground(WHITE);

        BeginMode2D(camera);
        draw();
        EndMode2D();

        draw_ui();

        EndDrawing();
    }

    rlImGuiEnd();
}