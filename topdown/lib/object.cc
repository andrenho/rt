#include "object.hh"

namespace topdown {

std::optional<CastHit> Object::cast(b2Vec2 target, float max_distance)
{
    std::optional<CastHit> hit;

    // TODO - adjust target by the max distance

    // cast ray
    struct Context { Object* this_; b2Vec2 target; std::optional<CastHit>& hit; };
    Context ctx { this, target, hit };

    b2World_CastRay(get_world_id(), get_center(), target - get_center(), {},
        [](b2ShapeId shapeId, b2Vec2 point, b2Vec2, float, void* context) {
            auto ctx = (Context *) context;
            void* data = b2Shape_GetUserData(shapeId);
            if (data && data != ctx->this_) {
                float length = b2Length(point - ctx->target);
                if (!ctx->hit || length < (*ctx->hit).length) {
                    auto obj = (Object *) data;
                    ctx->hit.emplace(obj, point, length);
                }
            }
            return 1.f;
        }, &ctx);

    return hit;
}

}