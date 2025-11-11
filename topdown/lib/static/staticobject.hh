#ifndef TOPDOWN_STATICOBJECT_HH
#define TOPDOWN_STATICOBJECT_HH

#include "../object.hh"

namespace topdown {

class StaticObject : public Object {
public:
    StaticObject(class World const& world, Shape const& shape) : StaticObject(world, std::vector<Shape>{ shape }) {}
    StaticObject(class World const& world, std::vector<Shape> const& shape);
    ~StaticObject() override;

    void shapes(std::vector<Shape>& shp) const override;

private:
    std::vector<Shape> shapes_ {};
    std::vector<b2ShapeId> shape_ids_ {};
};

}

#endif //TOPDOWN_STATICOBJECT_HH