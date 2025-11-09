#include "world.hh"

#include <raylib.h>

int main()
{
    topdown::World world;
    auto* car = world.add_object<topdown::Vehicle>(b2Vec2 { 0, 0 }, topdown::vehicle::Car);

    InitWindow(1600, 900, "topdown-test");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        world.step();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
}