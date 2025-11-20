#ifndef MAP_HH
#define MAP_HH

#include <utility>
#include <random>
#include <vector>

#include "geometry/point.hh"
#include "geometry/shapes.hh"

namespace map {

struct MapConfig {
    int    seed;
    int    map_w;
    int    map_h;
    int    point_density;
    float  point_randomness;
    int    polygon_relaxation_steps;
};

struct Biome {
    enum Type { Unknown, Ocean };

    geo::Point   original_point { 0, 0 };
    geo::Point   center_point { 0, 0 };
    geo::Polygon polygon {};
    float        elevation {};
    Type         type = Biome::Type::Unknown;
};

struct MapOutput {
    size_t w = 0, h = 0;
    std::vector<Biome> biomes {};
};

MapOutput create(MapConfig const& cfg);

} // map

#endif //MAP_HH
