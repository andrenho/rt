#ifndef RT_PHYSICALMAP_HH
#define RT_PHYSICALMAP_HH

#include "map.hh"

#include <unordered_map>
#include <utility>
#include <variant>

namespace map {

struct PhysicalMap {

    struct Terrain {
        geo::Shape  shape;
        bool        passable;
        Biome::Type terrain_type;
        std::unordered_map<geo::Point, uint8_t> static_features {};
    };

    int                                     w, h;
    std::vector<geo::Shape>                 water;
    std::vector<Terrain>                    terrains;
    std::vector<geo::Shape>                 unpassable_terrains;
    std::vector<geo::Shape>                 roads;
};

PhysicalMap generate_physical_map(Map const& map, size_t seed);

} // map

#endif //RT_PHYSICALMAP_HH
