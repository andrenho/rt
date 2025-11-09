#ifndef TOPDOWN_OBJECT_HH
#define TOPDOWN_OBJECT_HH

#include "box2d/box2d.h"

class Object {
public:
    virtual ~Object() = default;

protected:
    Object(b2BodyId id) : id_(id) {}

    b2BodyId id_;
};


#endif //TOPDOWN_OBJECT_HH
