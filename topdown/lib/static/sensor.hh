#ifndef TOPDOWN_SENSOR_HH
#define TOPDOWN_SENSOR_HH

#include <optional>

#include "staticobject.hh"
#include "../sensormodifier.hh"

namespace topdown {

class Sensor : public StaticObject {
public:
    Sensor(class World const& world, std::vector<Shape> const& shapes, std::optional<SensorModifier> const& sensor_modifier={})
            : StaticObject(world, shapes, true), sensor_modifier_(sensor_modifier) {}
    Sensor(class World const& world, Shape const& shape, std::optional<SensorModifier> const& sensor_modifier={})
            : Sensor(world, std::vector<Shape> { shape }, sensor_modifier) {}

    [[nodiscard]] std::optional<SensorModifier> const& sensor_modifier() const { return sensor_modifier_; }

    [[nodiscard]] bool is_sensor() const override { return true; }

private:
    std::optional<SensorModifier> sensor_modifier_;
};

}

#endif //TOPDOWN_SENSOR_HH
