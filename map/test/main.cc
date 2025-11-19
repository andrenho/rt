#include <cstdlib>
#include <ctime>

#include <optional>

#include <raylib.h>
#include <raymath.h>

#include "rlImGui.h"
#include "imgui.h"

#include "map/map.hh"
#include "geometry/shapes.hh"

static bool show_demo_window = false;
static Camera2D camera { { 0, 0 }, { 0, 0 }, 0, 1.0f };
static map::MapConfig map_config {
    .seed = 0,
    .map_w = 20000,
    .map_h = 20000,
    .point_density = 500,
    .point_randomness = .5f,
};
static map::Map map_;

struct State {
    bool show_points;
} state = {
    .show_points = true,
};

static Vector2 V(geo::Point const& p) { return { p.x, p.y }; }

static void show_full_map()
{
    camera.target = { 0, 0 };
    camera.offset = { 0, 0 };
    camera.zoom = (float) GetScreenWidth() / map_.size().w;
}

static void draw_shape(geo::Shape const& shape, std::optional<Color> line_color={}, std::optional<Color> bg_color={})
{
    std::visit(overloaded {
            [&](geo::Polygon const& p) {
                if (bg_color) {
                    for (size_t i = 0; i < p.size() - 1; i++)
                        DrawTriangle(V(p.at(i)), V(p.at(i+1)), V(p.center()), *bg_color);
                    DrawTriangle(V(p.at(p.size() - 1)), V(p.at(0)), V(p.center()), *bg_color);
                }
                if (line_color) {
                    for (size_t i = 0; i < p.size(); i++) {
                        auto a = p[i], b = p[(i + 1) % p.size()];
                        DrawLineEx({ a.x, a.y }, { b.x, b.y }, 1.f, *line_color);
                    }
                }
            },
            [&](geo::Circle const& c) {
                if (bg_color)
                    DrawCircle((int) c.center.x, (int) c.center.y, c.radius, *bg_color);
                if (line_color)
                    DrawCircleLines((int) c.center.x, (int) c.center.y, c.radius, *line_color);
            },
    }, shape);
}

static void draw_points()
{
    for (auto const& p: map_.polygon_points)
        draw_shape(geo::Circle { p, 40.f }, BLACK, VIOLET);
}

void draw_ui()
{
    rlImGuiBegin();

    ImGui::Begin("Map");

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {

        if (ImGui::BeginTabItem("Map generation")) {

            ImGui::SeparatorText("Map definition");
            ImGui::InputInt("Seed", &map_config.seed); ImGui::SameLine();
            if (ImGui::Button("New seed"))
                map_config.seed = rand();
            ImGui::InputInt("Map width", &map_config.map_w);
            ImGui::InputInt("Map height", &map_config.map_h);

            ImGui::SeparatorText("Polygons");
            ImGui::SliderInt("Point density", &map_config.point_density, 300, 1500);

            ImGui::SeparatorText("Generate map");
            if (ImGui::Button("Generate map"))
                map_.initialize(map_config);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Visualization")) {
            ImGui::SeparatorText("Visualization");
            ImGui::Checkbox("Show center points", &state.show_points);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Other")) {
            if(ImGui::Button("Reset zoom"))
                show_full_map();
            if (ImGui::Button("Show demo window"))
                show_demo_window = true;
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    rlImGuiEnd();
}

static void handle_events()
{
    if (IsKeyDown(KEY_Q))
        exit(EXIT_SUCCESS);

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f/camera.zoom);
        camera.target = Vector2Add(camera.target, delta);
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0.f)
    {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.offset = GetMousePosition();
        camera.target = mouseWorldPos;

        float scale = 0.2f*wheel;
        camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 0, 4.0f);
    }
}

int main()
{
    srand(time(nullptr));
    map_config.seed = rand();

    map_.initialize(map_config);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1600, 900, "rt-map-test");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    show_full_map();

    while (!WindowShouldClose()) {

        handle_events();

        BeginDrawing();
        ClearBackground(WHITE);

        BeginMode2D(camera);
        if (state.show_points)
            draw_points();
        EndMode2D();

        draw_ui();

        EndDrawing();
    }

    rlImGuiEnd();
}