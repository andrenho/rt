#include "world.hh"

#include <raylib.h>

int main()
{
    topdown::World world;

    InitWindow(1600, 900, "topdown-test");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
}