#ifndef RT_PHYSICALMAP_HH
#define RT_PHYSICALMAP_HH

#include "map.hh"

namespace map {

struct PhysicalMap {
    enum class ObjectType { Road };
    struct Object {
        ObjectType type;
        geo::Shape shape;
    };

    std::vector<Object> objects;
};

PhysicalMap generate_physical_map(Map const& map);

} // map

#endif //RT_PHYSICALMAP_HH
