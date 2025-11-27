#ifndef RT_PHYSICALMAP_HH
#define RT_PHYSICALMAP_HH

#include "map.hh"

#include <variant>

namespace map {

struct PhysicalMap {
    enum class ObjectType { Road, Water, TerrainSensor, ImpassableTerrain, StaticFeature };
    struct Object {
        ObjectType type;
        geo::Shape shape;
        std::variant<std::monostate, Biome::Type> additional_info {};
    };

    std::vector<Object> objects;
};

PhysicalMap generate_physical_map(Map const& map);

} // map

#endif //RT_PHYSICALMAP_HH
