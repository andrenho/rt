#include <raylib.h>
#include "rlImGui.h"
#include "imgui.h"

#include "city/city.hh"
#include "geometry/shapes.hh"

static city::CityConfig city_config;

static Camera2D camera { { 0, 0 }, { 0, 0 }, 0, 1.0f };
static bool show_demo_window = false;
static std::vector<geo::Shape> obstacles;

struct State {
    enum RoadShape : int { Terminal, Across, T, Y, X };
    int area_size = 500;
    RoadShape road_shape = Terminal;
} state;

std::vector<geo::Shape> create_road()
{
    return {}; // TODO
}

static void reset_map()
{
     obstacles = create_road();
}

static void handle_events()
{
}

static void draw()
{
}

static void draw_ui()
{
    rlImGuiBegin();

    ImGui::Begin("City");

    ImGui::InputInt("Area size", &state.area_size);

    static const char* m_road_shapes[] = { "Terminal", "Across", "T shape", "Y shape", "X shape" };
    ImGui::Combo("Road shape", (int *) &state.road_shape, m_road_shapes, IM_ARRAYSIZE(m_road_shapes));

    if (ImGui::Button("Generate map with new seed")) {
        city_config.seed = rand();
        reset_map();
    }
    ImGui::SameLine();
    if (ImGui::Button("Show demo window"))
        show_demo_window = true;

    ImGui::End();

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