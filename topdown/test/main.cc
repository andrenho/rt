#include <algorithm>

#include <raylib.h>

#include "util.hh"
#include "world.hh"

using namespace topdown;

int main()
{
    World world;
    world.add_object<Vehicle>(b2Vec2 { 0, 0 }, vehicle::Car);

    InitWindow(1600, 900, "topdown-test");
    SetTargetFPS(60);

    Camera2D camera { { 0, 0 }, { -200, -200 }, 0, 4.0f };

    while (!WindowShouldClose()) {
        world.step();

        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(RAYWHITE);

        for (auto const& object: world.objects()) {
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

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();
}