#include "object.hh"

namespace topdown {

std::vector <Shape> Object::shapes() const
{
    std::vector <Shape> shapes;

    b2ShapeId s_ids[8];
    int n = b2Body_GetShapes(id_, s_ids, 8);
    for (int i = 0; i < n; ++i) {
        switch (b2Shape_GetType(s_ids[i])) {
            case b2_circleShape:
                shapes.emplace_back(Circle { .center = b2Body_GetPosition(id_), .radius = b2Shape_GetCircle(s_ids[i]).radius });
                break;
            case b2_polygonShape: {
                Polygon pp;
                b2Polygon poly = b2Shape_GetPolygon(s_ids[i]);
                for (int j = 0; j < poly.count; ++j) {
                    b2Vec2 v = b2TransformPoint(b2Body_GetTransform(id_), poly.vertices[j]);
                    pp.push_back(v);
                }
                shapes.emplace_back(std::move(pp));
                break;
            }
            case b2_chainSegmentShape:
            case b2_shapeTypeCount:
            case b2_capsuleShape:
            case b2_segmentShape:
                break;
        }
    }

    return shapes;
}

b2Vec2 Object::lateral_velocity() const
{
    b2Vec2 current_normal = b2Body_GetWorldVector(id_, { 1, 0 });
    return b2Dot(current_normal, b2Body_GetLinearVelocity(id_)) * current_normal;
}

b2Vec2 Object::forward_velocity() const
{
    b2Vec2 current_normal = b2Body_GetWorldVector(id_, { 0, 1 });
    return b2Dot(current_normal, b2Body_GetLinearVelocity(id_)) * current_normal;
}

float Object::speed() const
{
    b2Vec2 current_forward_normal = b2Body_GetWorldVector(id_, {0, 1 });
    return b2Dot(forward_velocity(), current_forward_normal);
}

}