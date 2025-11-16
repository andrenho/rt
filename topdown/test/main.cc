#include <algorithm>

#include <raylib.h>

#include "util.hh"
#include "world.hh"

#define DARK_MODE 0

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
                    DrawCircleLinesV({ c.center.x, c.center.y }, c.radius, color);
                },
                [&](Line const& ln) {
                    DrawLineEx({ ln.first.x, ln.first.y }, { ln.second.x, ln.second.y }, 0.3f, color);
                },
        }, shape);
    }
}

static void draw_points()
{
    for (float x = -300; x < 300; x += 20.f)
        for (float y = -300; y < 300; y += 20.f)
            DrawLineEx( { x, y }, { x+.3f, y+.3f }, 0.5f, GREEN);
    DrawLineEx({ 0, 2 }, { 0, -2 }, 1.f, DARKGREEN);
    DrawLineEx({ 2, 0 }, { -2, 0 }, 1.f, DARKGREEN);
}

int main()
{
    std::optional<Cast> last_cast {};

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

    Person* hero = world.add_object<Person>(b2Vec2 { -70, 0 });
    world.add_object<PushableObject>(Box({ -70, -20 }, { 4, 4 }), 1.f);
    world.add_object<PushableObject>(Box({ -90, -20 }, { 4, 4 }), 100.f);

    world.add_object<StaticObject>(Circle { { 0, -80 }, 5 });

    SetConfigFlags(FLAG_MSAA_4X_HINT);
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
        draw_points();
        for (auto const& object: world.static_objects())
            draw_object(object.get());
        for (auto const& object: world.dynamic_objects())
            draw_object(object.get(), object.get() == vehicles.at(current_vehicle) ? SPECIAL : DARK);
        if (last_cast) {
            DrawLineEx(
                    {last_cast->originator->center().x, last_cast->originator->center().y },
                    { last_cast->final_point.x, last_cast->final_point.y },
                    1.0f, RED);
            last_cast = {};
        }
        DrawText(TextFormat("Ice"), -250, -250, 2, DARK);
        DrawText(TextFormat("Dirt"), -100, -250, 2, DARK);
        DrawText(TextFormat("Asphalt"), 90, -250, 2, DARK);
        EndMode2D();

        DrawText(TextFormat("Speed: %d", (int) vehicles.at(current_vehicle)->speed()), 10, 10, 20, SPECIAL);
        DrawText("Press TAB to switch vehicles", 10, 30, 10, SPECIAL);
        DrawText("Use WASD to drive vehicle", 10, 40, 10, SPECIAL);
        DrawText("Use arrows to move unit", 10, 50, 10, SPECIAL);
        DrawText("Use right click to fire a shot", 10, 60, 10, SPECIAL);
        DrawText("Use middle click to fire a missile", 10, 70, 10, SPECIAL);
        DrawText("Press M to place a landmine", 10, 80, 10, SPECIAL);


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

        if (IsKeyPressed(KEY_TAB)) {
            ++current_vehicle;
            current_vehicle %= vehicles.size();
        }

        float x = IsKeyDown(KEY_LEFT) ? -1.f : (IsKeyDown(KEY_RIGHT) ? 1.f : 0.f);
        float y = IsKeyDown(KEY_UP) ? -1.f : (IsKeyDown(KEY_DOWN) ? 1.f : 0.f);
        hero->set_move(x, y);

        if (IsKeyDown(KEY_Q))
            break;

        // handle mouse
        auto mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            auto cast = hero->cast({ mouse_pos.x, mouse_pos.y }, 100.f);
            auto hit = cast.hit;
            if (!hit)
                printf("No hit!\n");
            else
                printf("Object %p hit at %f, %f (distance %f)!\n", (void const *) hit->object, hit->location.x, hit->location.y, hit->length);
            last_cast = cast;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
            vehicles.at(current_vehicle)->fire_missile({ mouse_pos.x, mouse_pos.y }, 3.5f, {});
        }
        if (IsKeyPressed(KEY_M)) {
            vehicles.at(current_vehicle)->place_explosive({});
        }

    }

    CloseWindow();
}

