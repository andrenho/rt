#ifndef TOPDOWN_STATICOBJECT_HH
#define TOPDOWN_STATICOBJECT_HH

#include "../object.hh"

namespace topdown {

class StaticObject : public Object {
public:
    StaticObject(class World const& world, Shape const& shape) : StaticObject(world, std::vector<Shape>{ shape }) {}
    StaticObject(class World const& world, std::vector<Shape> const& shapes) : StaticObject(world, shapes, false) {}
    ~StaticObject() override;

    void shapes(std::vector<Shape>& shp) const override;

protected:
    StaticObject(class World const& world, std::vector<Shape> const& shape, bool sensor);

    [[nodiscard]] b2WorldId get_world_id() override;
    [[nodiscard]] b2Vec2 get_center() override;

private:
    std::vector<Shape> shapes_ {};
    std::vector<b2ShapeId> shape_ids_ {};
    b2Vec2 center_ { 0, 0 };
};

}

#endif //TOPDOWN_STATICOBJECT_HH