#ifndef RT_PHYSICALMAP_HH
#define RT_PHYSICALMAP_HH

#include "map.hh"

#include <unordered_map>
#include <utility>
#include <variant>

namespace map {

struct PhysicalMap {

    struct Object {
        geo::Shape shape;
        enum class Type { Terrain, Road, UnpassableArea, Wall, Sensor } type;
        union {
            nullptr_t def = nullptr;
            Biome::Type terrain_type;
            size_t sensor_id;
        };
        std::unordered_map<geo::Point, uint8_t> static_features {};
    };

    size_t w, h;
    size_t quads_w, quads_h;

    std::unordered_map<size_t, Object> objects;
    std::unordered_map<geo::UPoint, std::vector<size_t>> quadrants;
};

PhysicalMap generate_physical_map(Map const& map, size_t quadrant_sz, Random& random);

} // map

template<>
struct std::hash<map::PhysicalMap::Object> {
    std::size_t operator()(const map::PhysicalMap::Object& obj) const noexcept {
        return std::hash<geo::Shape>()(obj.shape);
    }
};

#endif //RT_PHYSICALMAP_HH
