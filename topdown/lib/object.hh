#ifndef TOPDOWN_OBJECT_HH
#define TOPDOWN_OBJECT_HH

#include <limits>
#include <optional>

#include "shapes.hh"

namespace topdown {

struct CastHit {
    class Object* object;
    b2Vec2        location;
    float         length;
};

class Object {
public:
    virtual ~Object() = default;
    virtual void shapes(std::vector<Shape>& shp) const = 0;

    std::optional<CastHit> cast(b2Vec2 target, float max_distance=std::numeric_limits<float>::infinity());

protected:
    Object() = default;

    virtual b2WorldId get_world_id() = 0;
    virtual b2Vec2    get_center() = 0;
};

}

#endif //TOPDOWN_OBJECT_HH
