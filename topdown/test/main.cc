#include <algorithm>

#include <raylib.h>

#include "util.hh"
#include "world.hh"

#define DARK_MODE 1

#if DARK_MODE == 0
#  define LIGHT   RAYWHITE
#  define DARK    BLACK
#  define SPECIAL RED
#else
#  define LIGHT   DARKGRAY
#  define DARK    RAYWHITE
#  define SPECIAL ORANGE
#endif

using namespace topdown;

namespace terrain {

    constexpr SensorModifier Asphalt = {
        .acceleration = 1.f,
        .skid = 0.f,
    };

    [[maybe_unused]] constexpr SensorModifier Dirt = {
        .acceleration = .7f,
        .skid = .5f,
    };

    constexpr SensorModifier Ice = {
        .acceleration = .6f,
        .skid = 1.2f,
    };
}

namespace vehicle {

    constexpr VehicleConfig Beetle = {
        .h = 4.f,
        .w = 2.f,
        .acceleration = 15.f,
        .wheelbase = 2.f,
        .skid = .2f,
        .drag = 1.f,
        .steering = 1.f,
        .fixed_acceleration = false,
        .n_wheels = 4,
    };

    constexpr VehicleConfig Car = {
        .h = 5.f,
        .w = 2.f,
        .acceleration = 20.f,
        .wheelbase = 3.f,
        .skid = 0.f,
        .drag = 1.f,
        .steering = 1.f,
        .fixed_acceleration = false,
        .n_wheels = 4,
    };

    constexpr VehicleConfig Motorcycle = {
        .h = 5.f,
        .w = .5f,
        .acceleration = 20.f,
        .wheelbase = 3.f,
        .skid = .2f,
        .drag = .8f,
        .steering = 1.f,
        .fixed_acceleration = false,
        .n_wheels = 2,
    };

    constexpr VehicleConfig Tank = {
        .h = 6.f,
        .w = 6.f,
        .acceleration = 40.f,
        .wheelbase = 6.f,
        .skid = -5.0f,
        .drag = 3.f,
        .steering = 4.f,
        .fixed_acceleration = true,
        .n_wheels = 4,
    };

    constexpr VehicleConfig Semi = {
        .h = 5.f,
        .w = 3.f,
        .acceleration = 30.f,
        .wheelbase = 4.5f,
        .skid = -2.0f,
        .drag = 0.f,
        .steering = 2.5f,
        .fixed_acceleration = false,
        .n_wheels = 4,
    };

    constexpr VehicleConfig TruckLoad = {
        .h = 12.f,
        .w = 3.f,
        .acceleration = 20.f,
        .wheelbase = 10.f,
        .skid =  -2.0f,
        .drag = 3.f,
        .steering = 0.f,
        .fixed_acceleration = false,
        .n_wheels = 4,
    };

}

void draw_object(Object const* object, Color color=DARK)
{
    std::vector<Shape> shapes; object->shapes(shapes);
    for (auto const& shape: shapes) {
        std::visit(overloaded {
                [&](Polygon const& p) {
                    for (size_t i = 0; i < p.size(); i++) {
                        auto a = p[i], b = p[(i + 1) % p.size()];
                        DrawLineEx({ a.x, a.y }, { b.x, b.y }, 0.5f, color);
                    }
                },
                [&](Circle const& c) {
                    DrawCircleLines((int) c.center.x, (int) c.center.y, c.radius, color);
                },
                [&](Line const& ln) {
                    DrawLineEx({ ln.first.x, ln.first.y }, { ln.second.x, ln.second.y }, 0.3f, color);
                },
        }, shape);
    }
}

int main()
{
    World world;
    world.add_object<Sensor>(Box({ -260, -260 }, { 150, 250 }), terrain::Ice);
    world.add_object<Sensor>(Box({ 80, -260 }, { 150, 250 }), terrain::Asphalt);
    std::vector<Vehicle*> vehicles = {
        world.add_object<Vehicle>(b2Vec2 { -50, 0 }, vehicle::Beetle),
        world.add_object<Vehicle>(b2Vec2 { -10, 0 }, vehicle::Car),
        world.add_object<Vehicle>(b2Vec2 { 40, 0 }, vehicle::Motorcycle),
        world.add_object<Vehicle>(b2Vec2 { 80, 0 }, vehicle::Tank),
        world.add_object<Vehicle>(b2Vec2 { 120, 0 }, vehicle::Semi),
    };
    Vehicle* truckload = world.add_object<Vehicle>(b2Vec2 { 120, 20 }, vehicle::TruckLoad);
    vehicles.at(4)->attach(truckload);
    size_t current_vehicle = 0;

    InitWindow(1600, 900, "topdown-test");
    SetTargetFPS(60);

    Camera2D camera { { 0, 0 }, { -280, -280 }, 0, 3.0f };

    while (!WindowShouldClose()) {

        //
        // draw
        //

        BeginDrawing();

        BeginMode2D(camera);
        ClearBackground(LIGHT);
        for (auto const& object: world.static_objects())
            draw_object(object.get());
        for (auto const& object: world.dynamic_objects())
            draw_object(object.get(), object.get() == vehicles.at(current_vehicle) ? SPECIAL : DARK);
        DrawText(TextFormat("Ice"), -250, -250, 2, DARK);
        DrawText(TextFormat("Dirt"), -100, -250, 2, DARK);
        DrawText(TextFormat("Asphalt"), 90, -250, 2, DARK);
        EndMode2D();

        DrawText(TextFormat("Speed: %d", (int) vehicles.at(current_vehicle)->speed()), 10, 10, 20, SPECIAL);
        DrawText("Press TAB to switch vehicles", 10, 30, 10, SPECIAL);

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

        vehicles.at(current_vehicle)->set_accelerator(IsKeyDown(KEY_W));
        vehicles.at(current_vehicle)->set_reverse(IsKeyDown(KEY_S));
        if (IsKeyDown(KEY_A))
            vehicles.at(current_vehicle)->set_steering(-1);
        else if (IsKeyDown(KEY_D))
            vehicles.at(current_vehicle)->set_steering(1);
        else
            vehicles.at(current_vehicle)->set_steering(0);

        if (IsKeyDown(KEY_Q))
            break;

        if (IsKeyPressed(KEY_TAB))
            current_vehicle = (++current_vehicle) % vehicles.size();
    }

    CloseWindow();
}

