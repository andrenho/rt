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
    void setup() override;

    void shapes(std::vector<Shape>& shp) const override;

    [[nodiscard]] float speed() const;
    [[nodiscard]] b2BodyId const& id() const { return id_; }

    virtual void touch_sensor(Sensor* sensor) { touching_sensor_.insert(sensor); }
    virtual void untouch_sensor(Sensor* sensor) { touching_sensor_.erase(sensor); }

protected:
    [[nodiscard]] b2WorldId world_id() const override;
    [[nodiscard]] b2Vec2 center() const override;

    [[nodiscard]] Category category() const override { return Category::Dynamic; }
    [[nodiscard]] std::vector<Category> categories_contact() const override { return { Category::Dynamic, Category::Solid, Category::Missile, Category::Sensor, Category::Shrapnel }; }

protected:
    explicit DynamicObject(b2BodyId id);

    b2BodyId id_;
    std::unordered_set<Sensor*> touching_sensor_ {};

    [[nodiscard]] b2Vec2 lateral_velocity() const;
    [[nodiscard]] b2Vec2 forward_velocity() const;

    [[nodiscard]] b2ShapeDef default_shape();
};

}

#endif //TOPDOWN_DYMAMICOBJECT_HH
