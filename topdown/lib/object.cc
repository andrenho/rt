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

}