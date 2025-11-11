#ifndef TOPDOWN_OBJECT_HH
#define TOPDOWN_OBJECT_HH

#include "box2d/box2d.h"

#include "shapes.hh"

namespace topdown {

class Object {
public:
    virtual ~Object();

    virtual void shapes(std::vector<Shape>& shp) const;

    virtual void step() {}

    [[nodiscard]] float speed() const;
    [[nodiscard]] b2BodyId const& id() const { return id_; }

protected:
    explicit Object(b2BodyId id) : id_(id) {}

    [[nodiscard]] b2Vec2 lateral_velocity() const;
    [[nodiscard]] b2Vec2 forward_velocity() const;

    b2BodyId id_;
};

}

#endif //TOPDOWN_OBJECT_HH
