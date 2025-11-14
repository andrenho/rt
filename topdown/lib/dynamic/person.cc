#include "person.hh"

#include "../world.hh"

namespace topdown {

Person::Person(World const& world, b2Vec2 initial_pos)
        : DynamicObject(build_body(world, initial_pos))
{

}

void Person::step()
{
    b2Body_SetLinearVelocity(id_, { move_x_ * SPEED, move_y_ * SPEED });
    /*
    b2Vec2 vel = body_->GetLinearVelocity();

    b2Vec2 vel_change = speed_ - vel;
    b2Vec2 impulse = body_->GetMass() * vel_change;

    body_->ApplyLinearImpulse(impulse, body_->GetWorldCenter(), true);
     */

    /*
    // create target body
    target = cpBodyNew(INFINITY, INFINITY);
    cpBodySetPos(target, cpv(x, y));

    // create joint
    joint = cpSpaceAddConstraint(space,
            cpPivotJointNew2(target, body, cpvzero, cpvzero));
    joint->maxBias = 15.0f;
    joint->maxForce = 30000.0f;

    void
    Person::SetTarget(Point const& p)
    {
    cpBodySetPos(target, cpv(p.X(), p.Y()));
    }
    */
}

void Person::set_move(float x, float y)
{
    move_x_ = x;
    move_y_ = y;
}

b2BodyId Person::build_body(World const& world, b2Vec2 initial_pos)
{
    // body
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_dynamicBody;
    body_def.position = initial_pos;
    body_def.motionLocks = { false, false, true };
    b2BodyId body_id = b2CreateBody(world.id(), &body_def);

    // shape
    b2Circle circle = { .center = { 0, 0 }, .radius = 1 };
    b2Polygon box = b2MakeRoundedBox(1, 1, 1);
    //b2Polygon box = b2MakeBox(1, 1);
    b2ShapeDef shape_def = default_shape();
    shape_def.density = 0.02f;
    shape_def.material.friction = 0.3f;
    b2CreateCircleShape(body_id, &shape_def, &circle);
    //b2CreatePolygonShape(body_id, &shape_def, &box);

    return body_id;
}

}
