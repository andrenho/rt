#include "object.hh"

#include "event.hh"

namespace topdown {

std::optional<CastHit> Object::cast(b2Vec2 target, float max_distance)
{
    std::vector<CastHit> hits;

    // cast ray
    b2QueryFilter filter;
    b2World_CastRay(get_world_id(), get_center(), target - get_center(), filter,
        [](b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context) {
            auto& hits = *(std::vector<ShotEvent> *) context;
            // TODO
            return 0.f;
        }, &hits);

    if (hits.empty())
        return {};

    // TODO - sort by distance and find first result
}

}