#include "object.hh"

#include "world.hh"
#include "dynamic/explosive.hh"

namespace topdown {

Cast Object::cast(b2Vec2 target, float max_distance) const
{
    Cast cast;

    // TODO - adjust target by the max distance
    float length = b2Length(center() - target);
    if (length > max_distance) {
        b2Vec2 dir = b2Normalize(target -center());
        target = center() + max_distance * dir;
    }

    cast.originator = this;
    cast.final_point = target;

    // cast ray
    struct Context { const Object* this_; b2Vec2 target; std::optional<Cast::Hit>& hit; };
    Context ctx { this, target, cast.hit };

    b2World_CastRay(world_id(), center(), target -center(), b2DefaultQueryFilter(),
        [](b2ShapeId shapeId, b2Vec2 point, b2Vec2, float, void* context) {

            auto c = (Context *) context;
            float hit_length = b2Length(c->this_->center() - point);
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

    if (cast.hit)
        cast.final_point = cast.hit->location;

    return cast;
}

Explosive* Object::fire_missile(b2Vec2 target, float power)
{
    return world().add_object<Explosive>(this, target, power);
}

Explosive* Object::place_landmine(float power)
{
    return world().add_object<Explosive>(this, power);
}

World& Object::world()
{
    return *(World *) b2World_GetUserData(world_id());
}

World const& Object::world() const
{
    return *(World *) b2World_GetUserData(world_id());
}

}