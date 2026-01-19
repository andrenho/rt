#ifndef MAP_HH
#define MAP_HH

#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

#include "random/random.hh"
#include "geometry/point.hh"
#include "geometry/shapes.hh"
#include "city/city.hh"

namespace map {

enum class CitySize : uint8_t { TradingPost, Village, Town, City };

struct MapConfig {
    int    map_w                        = 20000;
    int    map_h                        = 20000;
    int    point_density                = 500;
    float  point_randomness             = .7f;
    bool   polygon_relaxation           = true;
    float  ocean_elevation              = .4f;
    float  lake_threshold               = .28f;
    float  connect_city_distance        = 4500;
    float  road_weight_ocean            = 3.f;
    float  road_weight_forest           = 1.2f;
    float  road_weight_reuse            = .7f;
    std::vector<size_t> city_size { 6, 4, 3, 2 };

    [[nodiscard]] size_t number_of_cities() const;
};

struct Biome {
    Biome(geo::Point const& center_point_, geo::Shape polygon_)
        : center_point(center_point_), polygon(std::move(polygon_)) {}

    enum Type { Unknown, Ocean, Snow, Tundra, Desert, Grassland, Savannah, PineForest, Forest, RainForest };

    geo::Point   center_point { 0, 0 };
    geo::Shape   polygon {};
    float        elevation = .5f;
    float        moisture = .5f;
    Type         type = Biome::Type::Unknown;
    bool         contains_city = false;
    std::vector<Biome*> neighbours {};
};

struct City {
    Biome*                    biome;
    geo::Point                location;
    std::unordered_set<City*> connected_cities {};
    CitySize                  size;
    city::City                city {};
};

using RoadSegment = std::pair<geo::Point, geo::Point>;

struct Map {
    size_t w = 0, h = 0;
    size_t tiles_w = 0, tiles_h = 0;
    std::vector<std::unique_ptr<Biome>> biomes {};
    std::vector<std::unique_ptr<City>>  cities {};
    std::vector<RoadSegment>            road_segments {};
};

Map create(MapConfig const& cfg, Random& random);

} // map

#endif //MAP_HH
