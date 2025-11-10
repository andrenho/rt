#ifndef TOPDOWN_OBJECT_HH
#define TOPDOWN_OBJECT_HH

#include "box2d/box2d.h"

#include "shapes.hh"

namespace topdown {

class Object {
public:
    virtual ~Object() = default;

    [[nodiscard]] std::vector<Shape> shapes() const;

    void step();

    [[nodiscard]] float speed() const;

protected:
    explicit Object(b2BodyId id) : id_(id) {}

    struct Force {
        b2Vec2 point;
        b2Vec2 force;
        uint8_t color = 0;
    };
    [[nodiscard]] virtual std::vector<Force> iteration() const = 0;

    [[nodiscard]] b2Vec2 lateral_velocity() const;
    [[nodiscard]] b2Vec2 forward_velocity() const;

    b2BodyId id_;

private:
    mutable std::vector<Force> forces_ {};
};

}

#endif //TOPDOWN_OBJECT_HH
