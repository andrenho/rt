#ifndef TOPDOWN_OBJECT_HH
#define TOPDOWN_OBJECT_HH

#include "shapes.hh"

namespace topdown {

class Object {
public:
    virtual ~Object() = default;
    virtual void shapes(std::vector<Shape>& shp) const = 0;

protected:
    Object() = default;
};

}

#endif //TOPDOWN_OBJECT_HH
