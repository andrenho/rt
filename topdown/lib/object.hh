#ifndef TOPDOWN_OBJECT_HH
#define TOPDOWN_OBJECT_HH

#include "box2d/box2d.h"

#include "shapes.hh"

namespace topdown {

class Object {
public:
    virtual ~Object() = default;

    [[nodiscard]] std::vector<Shape> shapes() const;

protected:
    explicit Object(b2BodyId id) : id_(id) {}

    b2BodyId id_;
};

}

#endif //TOPDOWN_OBJECT_HH
