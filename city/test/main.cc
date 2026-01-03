#include <optional>

#include <raylib.h>
#include "rlImGui.h"
#include "imgui.h"

#include "city/city.hh"
#include "geometry/shapes.hh"
#include "city/prepare.hh"

static constexpr float BUILDING_SMALL  = 10;
static constexpr float BUILDING_MEDIUM = 15;
static constexpr float BUILDING_LARGE  = 20;

static std::vector<city::BuildingConfig> city_size[] = {
    {
        { .size = BUILDING_SMALL,  .count = 3 },
        { .size = BUILDING_MEDIUM, .count = 2 },
        { .size = BUILDING_LARGE,  .count = 0 },
    },
    {
        { .size = BUILDING_SMALL,  .count = 4 },
        { .size = BUILDING_MEDIUM, .count = 3 },
        { .size = BUILDING_LARGE,  .count = 1 },
    },
    {
        { .size = BUILDING_SMALL,  .count = 6 },
        { .size = BUILDING_MEDIUM, .count = 4 },
        { .size = BUILDING_LARGE,  .count = 2 },
    },
    {
        { .size = BUILDING_SMALL,  .count = 10 },
        { .size = BUILDING_MEDIUM, .count = 6 },
        { .size = BUILDING_LARGE,  .count = 3 },
    },
};

static city::CityConfig city_config {
    .seed = 0,
    .obstacles = {},
    .center = { 250, 250 },
    .buildings = city_size[1],
    .max_size = 200,
};
static city::City my_city;

static Camera2D camera { { 0, 0 }, { 0, 0 }, 0, 1.0f };
static bool show_demo_window = false;

enum CitySize : int {
    CS_Small, CS_Medium, CS_Large, CS_VeryLarge
};

struct State {
    int       area_size = 500;
    RoadShape road_shape = Terminal;
    CitySize  city_size = CS_Medium;
    bool      draw_original_poisson_disks = true;
} state;

static Vector2 V(geo::Point const& p) { return { p.x, p.y }; }

static void reset_map()
{
    std::mt19937 rng(city_config.seed);

    city_config.center = { (float) state.area_size / 2.f, (float) state.area_size / 2.f };
    city_config.obstacles = create_road(rng, state.road_shape, (float) state.area_size);
    city_config.buildings = city_size[state.city_size];

    my_city = city::generate_city(city_config);

    camera.zoom = (float) GetScreenHeight() / (float) state.area_size;
}

static void handle_events()
{
    if (IsKeyDown(KEY_Q))
        exit(EXIT_SUCCESS);
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
    for (auto const& obstacle: city_config.obstacles)
        draw_shape(obstacle, {}, ORANGE);
    if (state.draw_original_poisson_disks) {
        for (auto const& disk: my_city.original_poisson_disks) {
            draw_shape(geo::Shape::Circle(disk.center(), 2.f), LIGHTGRAY, LIGHTGRAY);
            draw_shape(disk, LIGHTGRAY);
        }
    }
}

static void draw_ui()
{
    rlImGuiBegin();

    ImGui::Begin("City");

    ImGui::SeparatorText("City area");
    ImGui::InputInt("Area size", &state.area_size);

    ImGui::SeparatorText("Road");
    static const char* m_road_shapes[] = { "Terminal", "Across", "Y shape", "Two lines across" };
    ImGui::Combo("Road shape", (int *) &state.road_shape, m_road_shapes, IM_ARRAYSIZE(m_road_shapes));

    ImGui::SeparatorText("City area");
    ImGui::InputFloat("Max building area", &city_config.max_size, .1f, .0f, "%.f");
    static const char* m_city_sizes[] = { "Small", "Medium", "Large", "Very Large" };
    ImGui::Combo("City size", (int *) &state.city_size, m_city_sizes, IM_ARRAYSIZE(m_city_sizes));

    ImGui::SeparatorText("View");
    ImGui::Checkbox("Original poisson disks", &state.draw_original_poisson_disks);

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

    reset_map();

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