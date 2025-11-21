#ifndef MAP_HH
#define MAP_HH

#include <utility>
#include <random>
#include <vector>

#include "geometry/point.hh"
#include "geometry/shapes.hh"

namespace map {

struct MapConfig {
    int    seed                         = rand();
    int    map_w                        = 20000;
    int    map_h                        = 20000;
    int    point_density                = 500;
    float  point_randomness             = .7f;
    int    polygon_relaxation_steps     = 1;
    float  ocean_elevation              = .4f;
    float  lake_threshold               = .28f;
};

struct Biome {
    enum Type { Unknown, Ocean, Snow, Tundra, Desert, Grassland, Savannah, PineForest, Forest, RainForest };

    geo::Point   original_point { 0, 0 };
    geo::Point   center_point { 0, 0 };
    geo::Polygon polygon {};
    float        elevation = .5f;
    float        moisture = .5f;
    Type         type = Biome::Type::Unknown;
};

struct MapOutput {
    size_t w = 0, h = 0;
    std::vector<Biome> biomes {};
};

MapOutput create(MapConfig const& cfg);

} // map

#endif //MAP_HH
