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
static map::MapConfig map_config {};

struct State {
    enum PolygonFill : int { None, Elevation, Oceans };
    bool        show_points;
    bool        show_polygons;
    PolygonFill polygon_fill;
} state = {
    .show_points = false,
    .show_polygons = true,
    .polygon_fill = State::PolygonFill::Oceans,
};

map::MapOutput map_;

static Vector2 V(geo::Point const& p) { return { p.x, p.y }; }

static void reset_map()
{
    map_ = map::create(map_config);
}

static void show_full_map()
{
    camera.target = { 0, 0 };
    camera.offset = { 0, 0 };
    camera.zoom = (float) GetScreenWidth() / (float) map_.w;
}

static void draw_shape(geo::Shape const& shape, std::optional<Color> line_color={}, std::optional<Color> bg_color={})
{
    std::visit(overloaded {
            [&](geo::Polygon const& p) {
                if (bg_color) {
                    for (size_t i = 0; i < p.size() - 1; i++)
                        DrawTriangle(V(p.at(i+1)), V(p.at(i)), V(p.center()), *bg_color);
                    DrawTriangle(V(p.at(0)), V(p.at(p.size() - 1)), V(p.center()), *bg_color);
                }
                if (line_color) {
                    for (size_t i = 0; i < p.size(); i++) {
                        auto a = p[i], b = p[(i + 1) % p.size()];
                        DrawLineEx({ a.x, a.y }, { b.x, b.y }, 1.f / camera.zoom, *line_color);
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
    for (auto const& biome: map_.biomes)
        draw_shape(geo::Circle { biome.original_point, 40.f }, BLACK, VIOLET);
}

static void draw_polygons()
{
    for (auto const& biome: map_.biomes) {
        switch (state.polygon_fill) {
            case State::PolygonFill::None:
                draw_shape(biome.polygon, BLACK);
                break;
            case State::PolygonFill::Elevation: {
                draw_shape(biome.polygon, BLACK, Color { 0, 0, 0, (uint8_t) (255.f - 255.f * biome.elevation ) });
                break;
            }
            case State::PolygonFill::Oceans:
                draw_shape(biome.polygon, BLACK, biome.type == map::Biome::Ocean ? SKYBLUE : BROWN);
        }
    }
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
            ImGui::SliderInt("Point density", &map_config.point_density, 100, 1500);
            ImGui::SliderFloat("Point randomness", &map_config.point_randomness, 0.0f, 1.0f, "%.3f");
            ImGui::SliderInt("Relaxation steps", &map_config.polygon_relaxation_steps, 0, 10);

            ImGui::SeparatorText("Terrain");
            ImGui::SliderFloat("Ocean elevation", &map_config.ocean_elevation, 0.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("Lake threshold", &map_config.lake_threshold, 0.0f, 1.0f, "%.3f");

            ImGui::SeparatorText("Generate map");
            if (ImGui::Button("Generate map"))
                reset_map();
            ImGui::SameLine();
            if (ImGui::Button("Generate map with new seed")) {
                map_config.seed = rand();
                reset_map();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Visualization")) {
            ImGui::SeparatorText("Visualization");
            ImGui::Checkbox("Show center points", &state.show_points);
            ImGui::Checkbox("Show polygons", &state.show_polygons);
            static const char* items[] = { "None", "Elevation", "Land/Water" };
            ImGui::Combo("Polygon fill", (int *) &state.polygon_fill, items, IM_ARRAYSIZE(items));
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

    if (IsKeyDown(KEY_G))
        reset_map();

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

    reset_map();

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
        if (state.show_polygons)
            draw_polygons();
        if (state.show_points)
            draw_points();
        EndMode2D();

        draw_ui();

        EndDrawing();
    }

    rlImGuiEnd();
}