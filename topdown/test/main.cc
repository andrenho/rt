#include <algorithm>

#include <raylib.h>

#include "util.hh"
#include "world.hh"
#include "wheel.hh"

using namespace topdown;

void draw_object(Object const* object)
{
    auto color = [](uint8_t n) {
        switch (n) {
            case 0: return BLACK;
            case 1: return RED;
            case 2: return BLUE;
            case 3: return DARKGREEN;
            case 4: return ORANGE;
            default: return PINK;
        }
    };

    std::vector<Shape> shapes; object->shapes(shapes);
    for (auto const& shape: shapes) {
        std::visit(overloaded {
                [](Polygon const& p) {
                    for (size_t i = 0; i < p.size(); i++) {
                        auto a = p[i], b = p[(i + 1) % p.size()];
                        DrawLineEx({ a.x, a.y }, { b.x, b.y }, 0.5f, BLACK);
                    }
                },
                [](Circle const& p) {
                    DrawCircleLines((int) p.center.x, (int) p.center.y, p.radius, BLACK);
                },
                [&](Line const& ln) {
                    DrawLineEx({ ln.first.x, ln.first.y }, { ln.second.x, ln.second.y }, 0.3f, color(shape.color));
                },
        }, shape.shape);
    }
}

int main()
{
    World world;
    auto car = world.add_object<Vehicle>(b2Vec2 { 0, 0 }, vehicle::Car);

    InitWindow(1600, 900, "topdown-test");
    SetTargetFPS(60);

    Camera2D camera { { 0, 0 }, { -200, -200 }, 0, 4.0f };

    while (!WindowShouldClose()) {

        BeginDrawing();

        BeginMode2D(camera);
        ClearBackground(RAYWHITE);
        for (auto const& object: world.objects())
            draw_object(object.get());
        EndMode2D();

        DrawText(TextFormat("Speed: %f", car->speed()), 10, 10, 20, RED);
        EndDrawing();

        world.step();

        car->set_accelerator(IsKeyDown(KEY_UP));
        car->set_breaks(IsKeyDown(KEY_DOWN));
        if (IsKeyDown(KEY_LEFT))
            car->set_steering(-1);
        else if (IsKeyDown(KEY_RIGHT))
            car->set_steering(1);
        else
            car->set_steering(0);

        if (IsKeyDown(KEY_Q))
            exit(0);

    }

    CloseWindow();
}

