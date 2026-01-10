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
        enum class Type { Terrain, Road, UnpassableArea } type;
        Biome::Type terrain_type = Biome::Unknown;
        std::unordered_map<geo::Point, uint8_t> static_features {};
    };

    size_t w, h;
    size_t quads_w, quads_h;

    std::unordered_map<size_t, Object> objects;
    std::unordered_map<geo::UPoint, std::vector<size_t>> quadrants;
};

PhysicalMap generate_physical_map(Map const& map, size_t seed, size_t quadrant_sz);

} // map

template<>
struct std::hash<map::PhysicalMap::Object> {
    std::size_t operator()(const map::PhysicalMap::Object& obj) const noexcept {
        return std::hash<geo::Shape>()(obj.shape);
    }
};

#endif //RT_PHYSICALMAP_HH
