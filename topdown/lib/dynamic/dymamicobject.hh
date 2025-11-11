#ifndef TOPDOWN_DYMAMICOBJECT_HH
#define TOPDOWN_DYMAMICOBJECT_HH

#include "box2d/box2d.h"

#include "../object.hh"
#include "../shapes.hh"

namespace topdown {

class DynamicObject : public Object {
public:
    virtual ~DynamicObject();

    virtual void step() {}

    void shapes(std::vector<Shape>& shp) const override;

    [[nodiscard]] float speed() const;
    [[nodiscard]] b2BodyId const& id() const { return id_; }

protected:
    explicit DynamicObject(b2BodyId id) :id_(id) {}

    [[nodiscard]] b2Vec2 lateral_velocity() const;
    [[nodiscard]] b2Vec2 forward_velocity() const;

    b2BodyId id_;
};

}

#endif //TOPDOWN_DYMAMICOBJECT_HH
