#include <cstdlib>
#include <ctime>

#include <optional>

#include <raylib.h>
#include <raymath.h>

#include "rlImGui.h"
#include "imgui.h"

#include "map/quadrants.hh"
#include "geometry/shapes.hh"

static map::Map map_;
static map::PhysicalMap pmap;
static map::Quadrants quadrants;

static bool show_demo_window = false;
static Camera2D camera { { 0, 0 }, { 0, 0 }, 0, 1.0f };
static map::MapConfig map_config {};
static Vector2 mouse_world_pos { 0, 0 };

struct State {
    enum PolygonFill : int { None, Elevation, Moisture, Oceans, Biomes };
    enum MapType : int { Political, Physical, Quadrants };
    MapType     map_type;
    bool        show_points;
    bool        show_polygons;
    PolygonFill polygon_fill;
    bool        show_city_locations;
    bool        show_connected_cities;
    bool        show_roads;
    int         quadrant_size = 500;
} state = {
    .map_type = State::MapType::Quadrants,
    .show_points = false,
    .show_polygons = true,
    .polygon_fill = State::PolygonFill::Biomes,
    .show_city_locations = true,
    .show_connected_cities = false,
    .show_roads = true,
};

                                         // Unknown, Ocean, Snow, Tundra, Desert, Grassland, Savannah, PineForest, Forest, RainForest };
static std::vector<Color> biome_colors = { BROWN, SKYBLUE, RAYWHITE, Color {0, 150, 150, 255}, BEIGE, GREEN, BROWN, DARKGREEN, Color {0, 149, 70, 255}, Color {0, 170, 90, 255}};

static Vector2 V(geo::Point const& p) { return { p.x, p.y }; }

static void reset_map()
{
    map_ = map::create(map_config);
    pmap = map::generate_physical_map(map_, map_config.seed + 1);
    quadrants = map::generate_quadrants(pmap, state.quadrant_size);
}

static void show_full_map()
{
    camera.target = { 0, 0 };
    camera.offset = { 0, 0 };
    camera.zoom = (float) GetScreenWidth() / (float) map_.w;
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

static void draw_points()
{
    for (auto const& biome: map_.biomes)
        draw_shape(geo::shape::Circle { biome->center_point, 40.f }, BLACK, VIOLET);
}

static void draw_biome_polygons()
{
    for (auto const& biome: map_.biomes) {
        switch (state.polygon_fill) {
            case State::PolygonFill::None:
                draw_shape(biome->polygon, BLACK);
                break;
            case State::PolygonFill::Elevation:
                draw_shape(biome->polygon, BLACK, Color { 0, 0, 0, (uint8_t) (255.f - 255.f * biome->elevation ) });
                break;
            case State::PolygonFill::Moisture:
                draw_shape(biome->polygon, BLACK, Color { 0, 0, 0, (uint8_t) (255.f - 255.f * biome->moisture ) });
                break;
            case State::PolygonFill::Oceans:
                draw_shape(biome->polygon, BLACK, biome->type == map::Biome::Ocean ? SKYBLUE : BROWN);
                break;
            case State::PolygonFill::Biomes:
                draw_shape(biome->polygon, BLACK, biome_colors.at((int) biome->type));
                break;
        }
        if (biome->contains_city && state.show_city_locations)
            draw_shape(biome->polygon, BLACK, PURPLE);
    }
}

static void draw_city_connections()
{
    for (auto const& city: map_.cities)
        for (auto const& other_city: city->connected_cities)
            draw_shape(geo::shape::Line { city->location, other_city->location }, RED, {}, 1.5f);
}

static void draw_roads()
{
    for (auto const& road: map_.road_segments)
        draw_shape(geo::shape::Line { road.first, road.second }, BLACK, {}, 3.f);
}

static void draw_physical_map(map::PhysicalMap const& pmap_)
{
    // int ts = (1.f / (float) camera.zoom) * 2.f;
    int ts = 6;
    for (auto const& t: pmap_.terrains) {
        if (t.passable)
            draw_shape(t.shape, {}, biome_colors.at(t.terrain_type));
        else
            draw_shape(t.shape, {}, BLACK);
        for (auto const& [point, _]: t.static_features) {
            DrawTriangle({ point.x, point.y + ts }, { point.x - ts, point.y + ts }, { point.x + ts, point.y + ts }, BLACK);
            DrawTriangle({ point.x, point.y - ts }, { point.x - ts, point.y + ts }, { point.x + ts, point.y + ts }, BLACK);
        }
    }

    for (auto const& shape: pmap_.water)
        draw_shape(shape, {}, SKYBLUE);

    for (auto const& shape: pmap_.roads)
        draw_shape(shape, DARKGRAY, DARKGRAY, 4.f);
}

static void draw_quadrants_grid()
{
    for (int x = 0; x <= map_.w; x += state.quadrant_size)
        DrawLine(x, 0, x, (int) map_.h, BLACK);

    for (int y = 0; y <= map_.h; y += state.quadrant_size)
        DrawLine(0, y, (int) map_.w, y, BLACK);
}

static void draw_visible_quadrants()
{
    auto [mx, my] = GetScreenToWorld2D(GetMousePosition(), camera);
    int tx = (int) mx / state.quadrant_size;
    int ty = (int) my / state.quadrant_size;

    auto it = quadrants.find({ tx, ty });
    if (it != quadrants.end())
        draw_physical_map(it->second);
}

static void draw()
{
    switch (state.map_type) {
    case State::Political:
        if (state.show_polygons)
            draw_biome_polygons();
        if (state.show_points)
            draw_points();
        if (state.show_connected_cities)
            draw_city_connections();
        if (state.show_roads)
            draw_roads();
        break;
    case State::Physical:
        draw_physical_map(pmap);
        break;
    case State::Quadrants:
        draw_visible_quadrants();
        draw_quadrants_grid();
        break;
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
            ImGui::SliderInt("Point density", &map_config.point_density, 100, 2000);
            ImGui::SliderFloat("Point randomness", &map_config.point_randomness, 0.0f, 1.0f, "%.3f");
            ImGui::Checkbox("Relaxation steps", &map_config.polygon_relaxation);

            ImGui::SeparatorText("Terrain");
            ImGui::SliderFloat("Ocean elevation", &map_config.ocean_elevation, 0.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("Lake threshold", &map_config.lake_threshold, 0.0f, 1.0f, "%.3f");

            ImGui::SeparatorText("Cities & Roads");
            ImGui::SliderInt("Number of cities", &map_config.number_of_cities, 3, 50);
            ImGui::SliderFloat("Connected city distance", &map_config.connect_city_distance, 0.0f, 20000.0f, "%.0f");
            ImGui::SliderFloat("Road weight - Ocean", &map_config.road_weight_ocean, 0.f, 5.f, "%.1f");
            ImGui::SliderFloat("Road weight - Forest", &map_config.road_weight_forest, 0.f, 5.f, "%.1f");
            ImGui::SliderFloat("Road weight - Reuse", &map_config.road_weight_reuse, 0.f, 5.f, "%.1f");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Visualization")) {
            ImGui::SeparatorText("Map type");
            static const char* m_items[] = { "Political", "Physical", "Quadrants" };
            ImGui::Combo("Map Type", (int *) &state.map_type, m_items, IM_ARRAYSIZE(m_items));

            ImGui::SeparatorText("Visualization");
            ImGui::Checkbox("Show center points", &state.show_points);
            ImGui::Checkbox("Show polygons", &state.show_polygons);
            static const char* items[] = { "None", "Elevation", "Moisture", "Land/Water", "Biomes" };
            ImGui::Combo("Polygon fill", (int *) &state.polygon_fill, items, IM_ARRAYSIZE(items));
            ImGui::Checkbox("Show city locations", &state.show_city_locations);
            ImGui::Checkbox("Show connected cities", &state.show_connected_cities);
            ImGui::Checkbox("Show roads", &state.show_roads);
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

    ImGui::SeparatorText("Generate map");
    if (ImGui::Button("Generate map"))
        reset_map();
    ImGui::SameLine();
    if (ImGui::Button("Generate map with new seed")) {
        map_config.seed = rand();
        reset_map();
    }

    ImGui::LabelText("Position", "%.0f, %.0f", mouse_world_pos.x, mouse_world_pos.y);

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

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f/camera.zoom);
        camera.target = Vector2Add(camera.target, delta);
    }

    float wheel = GetMouseWheelMove();
    mouse_world_pos = GetScreenToWorld2D(GetMousePosition(), camera);
    if (wheel < 0.f || wheel > 0.f) {
        camera.offset = GetMousePosition();
        camera.target = mouse_world_pos;

        float scale = 0.2f*wheel;
        camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 0, 16.0f);
    }
}

int main()
{
    srand((unsigned int) time(nullptr));
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
        draw();
        EndMode2D();

        draw_ui();

        EndDrawing();
    }

    rlImGuiEnd();
}