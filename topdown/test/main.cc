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
    world.add_object<Sensor>(Box({ -260, -260 }, { 150, 250 }), terrain::Ice);
    world.add_object<Sensor>(Box({ 80, -260 }, { 150, 250 }), terrain::Asphalt);
    auto car = world.add_object<Vehicle>(b2Vec2 { 0, 0 }, vehicle::Car);

    InitWindow(1600, 900, "topdown-test");
    SetTargetFPS(60);

    Camera2D camera { { 0, 0 }, { -280, -280 }, 0, 3.0f };

    while (!WindowShouldClose()) {

        //
        // draw
        //

        BeginDrawing();

        BeginMode2D(camera);
        ClearBackground(RAYWHITE);
        for (auto const& object: world.static_objects())
            draw_object(object.get());
        for (auto const& object: world.dynamic_objects())
            draw_object(object.get());
        DrawText(TextFormat("Ice"), -250, -250, 2, BLACK);
        DrawText(TextFormat("Dirt"), -100, -250, 2, BLACK);
        DrawText(TextFormat("Asphalt"), 90, -250, 2, BLACK);
        EndMode2D();

        DrawText(TextFormat("Speed: %f", car->speed()), 10, 10, 20, RED);

        EndDrawing();

        //
        // world step
        //

        for (auto const& event: world.step()) {
            printf("Event\n");
        }

        //
        // handle keyboard
        //

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

