#include "world.hh"

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

    Person* hero = world.add_object<Person>(b2Vec2 { -70, 0 });
    world.add_object<PushableObject>(Box({ -70, -20 }, { 4, 4 }), 1.f);
    world.add_object<PushableObject>(Box({ -90, -20 }, { 4, 4 }), 100.f);

    world.add_object<StaticObject>(Circle { { 0, -80 }, 5 });

    vehicles.at(current_vehicle)->set_accelerator(true);
    hero->set_move(1.f, 0.f);

    for (size_t i = 0; i < 100; ++i)
        world.step();

    vehicles.at(current_vehicle)->fire_missile({ -40, 0 }, 20.f, {});

    for (size_t i = 0; i < 100; ++i)
        world.step();
}
