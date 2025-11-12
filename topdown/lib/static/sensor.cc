#include "sensor.hh"

#include "../world.hh"

namespace topdown {

Sensor::Sensor(World const& world, std::vector<Shape> const& shape)
        : StaticObject(world, shape, true)
{
}

}