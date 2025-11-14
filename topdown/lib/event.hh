#ifndef TOPDOWN_EVENT_HH
#define TOPDOWN_EVENT_HH

#include <variant>
#include "object.hh"
#include "static/sensor.hh"

namespace topdown {

struct BeginSensorEvent {
    Sensor*        sensor;
    DynamicObject* object;
};

struct EndSensorEvent {
    Sensor*        sensor;
    DynamicObject* object;
};

struct HitEvent {
    Object* object1;
    Object* object2;
    float   approach_speed;
};

struct ShotEvent {
    Object* fired_by;
    Object* object_hit;
    b2Vec2  point_hit;
};

using Event = std::variant<BeginSensorEvent, EndSensorEvent, HitEvent, ShotEvent>;

}

#endif //TOPDOWN_EVENT_HH
