#ifndef TOPDOWN_DYMAMICOBJECT_HH
#define TOPDOWN_DYMAMICOBJECT_HH

#include "box2d/box2d.h"

#include "../object.hh"
#include "../shapes.hh"
#include "../static/sensor.hh"

#include <unordered_set>

namespace topdown {

class DynamicObject : public Object {
public:
    ~DynamicObject() override;

    virtual void step() {}

    void shapes(std::vector<Shape>& shp) const override;

    [[nodiscard]] float speed() const;
    [[nodiscard]] b2BodyId const& id() const { return id_; }

    virtual void touch_sensor(Sensor* sensor) { touching_sensor_.insert(sensor); }
    virtual void untouch_sensor(Sensor* sensor) { touching_sensor_.erase(sensor); }

protected:
    explicit DynamicObject(b2BodyId id) :id_(id) {}

    [[nodiscard]] b2Vec2 lateral_velocity() const;
    [[nodiscard]] b2Vec2 forward_velocity() const;

    b2BodyId id_;
    std::unordered_set<Sensor*> touching_sensor_ {};
};

}

#endif //TOPDOWN_DYMAMICOBJECT_HH
