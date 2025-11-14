#include "person.hh"

#include "../world.hh"

namespace topdown {

Person::Person(World const& world, b2Vec2 initial_pos)
        : DynamicObject(build_body(world, initial_pos))
{

}

void Person::step()
{
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
    // b2Circle circle = { .center = initial_pos, .radius = 1 };
    b2Polygon box = b2MakeBox(1, 1);
    b2ShapeDef shape_def = default_shape();
    shape_def.density = 1.0f;
    shape_def.material.friction = 0.3f;
    // b2CreateCircleShape(body_id, &shape_def, &circle);
    b2CreatePolygonShape(body_id, &shape_def, &box);

    return body_id;
}

}
