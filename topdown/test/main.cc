#include <algorithm>

#include <raylib.h>

#include "util.hh"
#include "world.hh"

using namespace topdown;

void draw_object(Object const* object)
{
    std::vector<Shape> shapes; object->shapes(shapes);
    for (auto const& shape: shapes) {
        std::visit(overloaded {
                [](Polygon const& p) {
                    for (size_t i = 0; i < p.size(); i++) {
                        auto a = p[i], b = p[(i + 1) % p.size()];
                        DrawLineEx({ a.x, a.y }, { b.x, b.y }, 0.5f, BLACK);
                    }
                },
                [](Circle const& c) {
                    DrawCircleLines((int) c.center.x, (int) c.center.y, c.radius, BLACK);
                },
                [&](Line const& ln) {
                    DrawLineEx({ ln.first.x, ln.first.y }, { ln.second.x, ln.second.y }, 0.3f, BLACK);
                },
        }, shape);
    }
}

int main()
{
    World world;
    // world.add_static_shape(Circle { .center = { 15, -15 }, .radius = 3 });
    world.add_object<Sensor>(Circle { .center = { 15, -15 }, .radius = 3 });
    auto car = world.add_object<Vehicle>(b2Vec2 { 0, 0 }, vehicle::Car);

    InitWindow(1600, 900, "topdown-test");
    SetTargetFPS(60);

    Camera2D camera { { 0, 0 }, { -200, -200 }, 0, 4.0f };

    while (!WindowShouldClose()) {

        BeginDrawing();

        BeginMode2D(camera);
        ClearBackground(RAYWHITE);
        for (auto const& object: world.static_objects())
            draw_object(object.get());
        for (auto const& object: world.dynamic_objects())
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
            break;
    }

    CloseWindow();
}

