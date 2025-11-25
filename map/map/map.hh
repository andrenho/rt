#ifndef MAP_HH
#define MAP_HH

#include <memory>
#include <unordered_set>
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
    int    number_of_cities             = 15;
    float  connect_city_distance        = 4500;
    float  road_weight_ocean            = 3.f;
    float  road_weight_forest           = 1.2f;
    float  road_weight_reuse            = .1f;
};

struct Biome {
    enum Type { Unknown, Ocean, Snow, Tundra, Desert, Grassland, Savannah, PineForest, Forest, RainForest };

    geo::Point   original_point { 0, 0 };
    geo::Point   center_point { 0, 0 };
    geo::Polygon polygon {};
    float        elevation = .5f;
    float        moisture = .5f;
    Type         type = Biome::Type::Unknown;
    bool         contains_city = false;
    bool         has_road = false;
    std::vector<Biome*> neighbours {};
};

struct City {
    Biome* biome;
    geo::Point location;
    std::unordered_set<City*> connected_cities {};
};

using RoadSegment = std::pair<geo::Point, geo::Point>;

struct MapOutput {
    size_t w = 0, h = 0;
    size_t tiles_w = 0, tiles_h = 0;
    std::vector<std::unique_ptr<Biome>> biomes {};
    std::vector<std::unique_ptr<City>> cities {};
    std::vector<RoadSegment> road_segments {};
};

MapOutput create(MapConfig const& cfg);

} // map

#endif //MAP_HH
