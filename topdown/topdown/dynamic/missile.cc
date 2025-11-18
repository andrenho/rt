#include "missile.hh"

#include "../world.hh"

namespace topdown {

Missile::Missile(World const& world, Object* originator, b2Vec2 target, float speed, ExplosiveDef const& explosive_def)
    : Explosive(world, originator, explosive_def), target_(target), initial_pos_(b2Body_GetWorldCenterOfMass(id_)),
      distance_sq_target_(b2LengthSquared(initial_pos_ - target_))
{
    b2Vec2 force = speed * 100.f * b2Normalize(target - b2Body_GetWorldCenterOfMass(id_));
    b2Body_SetLinearVelocity(id_, force);
}

void Missile::step()
{
    DynamicObject::step();

    float distance = b2LengthSquared(b2Body_GetWorldCenterOfMass(id_) - initial_pos_);
    if (distance > distance_sq_target_) {
        explode();
        schedule_myself_for_deletion();
    }
}

}