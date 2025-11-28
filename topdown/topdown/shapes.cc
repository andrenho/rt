#include "shapes.hh"

#include <stdexcept>

namespace topdown {

b2Vec2 b2vec(geo::Point const& p)
{
    return { p.x, p.y };
}

geo::Point point(b2Vec2 const& v)
{
    return { v.x, v.y };
}

b2ShapeId create_b2shape(b2BodyId body_id, geo::Shape const& shape, bool sensor, void* user_data)
{
    b2ShapeDef shape_def = b2DefaultShapeDef();
    if (sensor) {
        shape_def.isSensor = true;
    }
    shape_def.enableSensorEvents = true;
    shape_def.userData = user_data;

    return std::visit(overloaded {
        [&](geo::shape::Polygon const& polygon) {
            std::vector<b2Vec2> vecs;
            for (auto p: polygon)
                vecs.emplace_back(p.x, p.y);
            b2Hull hull = b2ComputeHull(vecs.data(), (int) vecs.size());
            b2Polygon poly = b2MakePolygon(&hull, 0);
            return b2CreatePolygonShape(body_id, &shape_def, &poly);
        },
        [&](geo::shape::Circle const& circle) {
            b2Circle c { { circle.center.x, circle.center.y }, circle.radius };
            return b2CreateCircleShape(body_id, &shape_def, &c);
        },
        [&](geo::shape::Capsule const& capsule) {
            b2Capsule c { { capsule.p1.x, capsule.p1.y }, { capsule.p2.x, capsule.p2.y }, capsule.radius };
            return b2CreateCapsuleShape(body_id, &shape_def, &c);
        },
        [&](geo::shape::Line const& line) {
            throw std::runtime_error("Lines not supported at the present moment");
            return b2CreateCircleShape(body_id, nullptr, nullptr);  // makes compiler happy
        },
    }, shape.for_visit());
}

}