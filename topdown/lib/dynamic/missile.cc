#include "missile.hh"

#include "../world.hh"

namespace topdown {

Missile::Missile(World const& world, Object* originator, b2Vec2 target, float speed, ExplosiveDef const& explosive_def)
    : Explosive(world, originator, explosive_def), target_(target)
{
    b2Vec2 force = speed * 100.f * b2Normalize(target - b2Body_GetWorldCenterOfMass(id_));
    b2Body_SetLinearVelocity(id_, force);
}

}