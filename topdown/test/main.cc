#include <algorithm>

#include <raylib.h>

#include "util.hh"
#include "world.hh"

using namespace topdown;

void draw_object(Object const* object)
{
    for (auto const& shape: object->shapes()) {
        std::visit(overloaded {
                [](Polygon const& p) {
                    for (size_t i = 0; i < p.size(); i++) {
                        auto a = p[i], b = p[(i + 1) % p.size()];
                        DrawLineEx({ a.x, a.y }, { b.x, b.y }, 0.5f, RED);
                    }
                },
                [](Circle const& p) {
                    DrawCircleLines((int) p.center.x, (int) p.center.y, p.radius, RED);
                },
        }, shape);
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
        world.step();

        BeginDrawing();

        BeginMode2D(camera);
        ClearBackground(RAYWHITE);
        for (auto const& object: world.objects())
            draw_object(object.get());
        EndMode2D();

        DrawText(TextFormat("Speed: %f", car->speed()), 10, 10, 20, RED);
        EndDrawing();

        car->set_accelerator(IsKeyDown(KEY_UP));
        car->set_breaks(IsKeyDown(KEY_DOWN));
    }

    CloseWindow();
}

