#include <optional>

#include <raylib.h>
#include "rlImGui.h"
#include "imgui.h"

#include "city/city.hh"
#include "geometry/shapes.hh"
#include "city/prepare.hh"

static city::CityConfig city_config;

static Camera2D camera { { 0, 0 }, { 0, 0 }, 0, 1.0f };
static bool show_demo_window = false;
static std::vector<geo::Shape> obstacles;

struct State {
    int area_size = 500;
    RoadShape road_shape = Terminal;
} state;

static Vector2 V(geo::Point const& p) { return { p.x, p.y }; }

static void reset_map()
{
    std::mt19937 rng(city_config.seed);
    obstacles = create_road(rng, state.road_shape, (float) state.area_size);

    camera.zoom = (float) GetScreenHeight() / (float) state.area_size;
}

static void handle_events()
{
}

static void draw_shape(geo::Shape const& shape, std::optional<Color> line_color={}, std::optional<Color> bg_color={}, float line_width=1.f)
{
    std::visit(overloaded {
            [&](geo::shape::Polygon const& p) {
                if (bg_color) {
                    for (size_t i = 0; i < p.size() - 1; i++)
                        DrawTriangle(V(p.at(i+1)), V(p.at(i)), V(shape.center()), *bg_color);
                    DrawTriangle(V(p.at(0)), V(p.at(p.size() - 1)), V(shape.center()), *bg_color);
                }
                if (line_color) {
                    for (size_t i = 0; i < p.size(); i++) {
                        auto a = p[i], b = p[(i + 1) % p.size()];
                        DrawLineEx({ a.x, a.y }, { b.x, b.y }, (1.f / camera.zoom) * line_width, *line_color);
                    }
                }
            },
            [&](geo::shape::Circle const& c) {
                if (bg_color)
                    DrawCircle((int) c.center.x, (int) c.center.y, c.radius, *bg_color);
                if (line_color)
                    DrawCircleLines((int) c.center.x, (int) c.center.y, c.radius, *line_color);
            },
            [&](geo::shape::Line const& ln) {
                DrawLineEx({ ln.p1.x, ln.p1.y }, { ln.p2.x, ln.p2.y }, (1.f / camera.zoom) * line_width, *line_color);
            },
            [&](geo::shape::Capsule const& c) {
                draw_shape(geo::shape::Circle { c.p1, c.radius }, line_color, bg_color, line_width);
                draw_shape(geo::shape::Circle { c.p2, c.radius }, line_color, bg_color, line_width);
                draw_shape(geo::Shape::ThickLine(c.p1, c.p2, c.radius), line_color, bg_color, line_width);
            },
    }, shape.for_visit());
}

static void draw()
{
    draw_shape(geo::Shape::Box(geo::Point(0, 0), geo::Point(state.area_size, state.area_size)),
            BLACK, {}, 3.f);
    for (auto const& obstacle: obstacles)
        draw_shape(obstacle, {}, ORANGE);
}

static void draw_ui()
{
    rlImGuiBegin();

    ImGui::Begin("City");

    ImGui::InputInt("Area size", &state.area_size);

    static const char* m_road_shapes[] = { "Terminal", "Across", "Y shape", "Two lines across" };
    ImGui::Combo("Road shape", (int *) &state.road_shape, m_road_shapes, IM_ARRAYSIZE(m_road_shapes));

    ImGui::Separator();
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

    camera.target = { 0, 0 };
    camera.offset = { 0, 0 };

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