#include "object.hh"

namespace topdown {

Cast Object::cast(b2Vec2 target, float max_distance)
{
    Cast cast;

    // TODO - adjust target by the max distance
    float length = b2Length(get_center() - target);
    if (length > max_distance) {
        // TODO - adjust target by the max distance
    }

    cast.originator = this;
    cast.final_point = target;

    // cast ray
    struct Context { Object* this_; b2Vec2 target; std::optional<Cast::Hit>& hit; };
    Context ctx { this, target, cast.hit };

    b2World_CastRay(get_world_id(), get_center(), target - get_center(), b2DefaultQueryFilter(),
        [](b2ShapeId shapeId, b2Vec2 point, b2Vec2, float, void* context) {

            auto c = (Context *) context;
            float hit_length = b2Length(c->this_->get_center() - point);
            void* data = b2Shape_GetUserData(shapeId);

            if (!data)
                return 1.f;  // shape doesn't have a user pointer
            if (data == c->this_)
                return 1.f;  // shape hit is the generator of the ray
            auto obj = (Object *) data;
            if (obj->is_sensor())
                return 1.f;  // ignore sensors

            if (!c->hit || hit_length < (*c->hit).length) {  // if there wasn't a previous hit, or hit is closer
                c->hit.emplace(obj, point, hit_length);
            }

            return 1.f;
        }, &ctx);

    return cast;
}

}