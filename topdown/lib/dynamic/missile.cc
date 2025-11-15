#include "missile.hh"

#include "../world.hh"

namespace topdown {

Missile::Missile(World const& world, Object* originator, b2Vec2 target, ExplosiveDef const& explosive_def)
    : Explosive(world, originator, explosive_def), target_(target)
{
}

}