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
    bool   polygon_relaxation           = true;
    float  ocean_elevation              = .4f;
    float  lake_threshold               = .28f;
    int    number_of_cities             = 15;
    float  connect_city_distance        = 4500;
    float  road_weight_ocean            = 3.f;
    float  road_weight_forest           = 1.2f;
    float  road_weight_reuse            = .7f;
};

enum BiomeType { Unknown, Ocean, Snow, Tundra, Desert, Grassland, Savannah, PineForest, Forest, RainForest };

class Minimap {
public:
    explicit Minimap(MapConfig const& config, std::mt19937& rng);

    struct Biome {
        Biome(geo::Point const& center_point_, geo::Shape polygon_)
                : center_point(center_point_), polygon(std::move(polygon_)) {}

        geo::Point   center_point { 0, 0 };
        geo::Shape   polygon {};
        float        elevation = .5f;
        float        moisture = .5f;
        BiomeType    type = BiomeType::Unknown;
        bool         contains_city = false;
        std::vector<Biome*> neighbours {};
    };

    struct City {
        Biome* biome;
        geo::Point location;
        std::unordered_set<City*> connected_cities {};
    };

    using RoadSegment = std::pair<geo::Point, geo::Point>;

    size_t w = 0, h = 0;
    std::vector<std::unique_ptr<Biome>> biomes {};
    std::vector<std::unique_ptr<City>> cities {};
    std::vector<RoadSegment> road_segments {};
};

} // map

#endif //MAP_HH
