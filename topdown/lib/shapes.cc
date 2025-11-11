#include "shapes.hh"

#include "util.hh"

namespace topdown {

b2ShapeId create_b2shape(b2BodyId body_id, topdown::Shape const& shape)
{
    b2ShapeDef shape_def = b2DefaultShapeDef();

    return std::visit(overloaded {
        [&](Polygon const& polygon) {
            std::vector<b2Vec2> vecs;
            for (auto p: polygon)
                vecs.emplace_back(p.x, p.y);
            b2Hull hull = b2ComputeHull(vecs.data(), (int) vecs.size());
            b2Polygon poly = b2MakePolygon(&hull, 0);
            return b2CreatePolygonShape(body_id, &shape_def, &poly);
        },
        [&](Circle const& circle) {
            b2Circle c { { circle.center.x, circle.center.y }, circle.radius };
            return b2CreateCircleShape(body_id, &shape_def, &c);
        },
        [&](Line const& ln) {
            b2Vec2 vecs[4] = { ln.first, ln.second, ln.first + b2Vec2 { 0, .01f }, ln.second + b2Vec2 { 0, .01f } };
            b2Hull hull = b2ComputeHull(vecs, 4);
            b2Polygon poly = b2MakePolygon(&hull, 0);
            return b2CreatePolygonShape(body_id, &shape_def, &poly);
        },
    }, shape);
}

}