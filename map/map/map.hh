#ifndef MAP_HH
#define MAP_HH

#include "geometry/point.hh"

namespace map {

struct MapConfig {
    geo::Size size;
};

class Map {
public:
    explicit Map(MapConfig const& cfg) : cfg_(cfg) {}

    [[nodiscard]] geo::Size size() const { return cfg_.size; }

private:
    MapConfig cfg_;
};

} // map

#endif //MAP_HH
