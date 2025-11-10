#include "world.hh"

int main()
{
    topdown::World world;
    auto* car = world.add_object<topdown::Vehicle>(b2Vec2 { 0, 0 }, topdown::vehicle::Car);

    world.step();
    world.step();
    world.step();
}
