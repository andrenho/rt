#include <algorithm>
#include <ranges>

#include "raylib.h"
#include "rlImGui.h"
#include "rlgl.h"
#include "imgui.h"

#include "geometry/shapes.hh"
#include "game/game.hh"

static bool show_demo_window = false;
static Camera2D camera { { 0, 0 }, { 0, 0 }, 0, 1.0f };
static game::GameConfig config {};
static game::Game game_ = game_init(config);

static void handle_events()
{
    if (IsKeyDown(KEY_Q))
        exit(EXIT_SUCCESS);
}

static void draw_shape(geo::Shape const& shape, std::optional<Color> line_color={}, std::optional<Color> bg_color={}, float line_width=1.f)
{
    auto V = [](geo::Point const& p) -> Vector2 { return { p.x, p.y }; };

    std::visit(overloaded {
            [&](geo::shape::Polygon const& p) {
                if (bg_color) {
                    std::vector<Vector2> points = p
                            | std::views::transform([&V](geo::Point const& pp) { return V(pp); })
                            | std::ranges::to<std::vector>();
                    DrawTriangleFan(points.data(), (int) points.size(), *bg_color);
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
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1600, 900, "rt-game-test");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    while (!WindowShouldClose()) {

        handle_events();

        BeginDrawing();
        ClearBackground(WHITE);

        rlDisableBackfaceCulling();

        BeginMode2D(camera);
        draw();
        EndMode2D();

        draw_ui();

        EndDrawing();
    }

    rlImGuiEnd();

}