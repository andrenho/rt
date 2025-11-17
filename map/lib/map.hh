#ifndef MAP_HH
#define MAP_HH

#include <vector>

#include "features.hh"
#include "geo/point.hh"

namespace map {

struct MapConfig {
};

class Map {
public:
    explicit Map(MapConfig const& cfg);

    [[nodiscard]] Bounds const& bounds() const { return bounds_; }
    [[nodiscard]] std::vector<Terrain> const& terrains() const { return terrains_; }

private:
    Bounds bounds_;
    std::vector<Terrain> terrains_;
};

} // map

#endif //MAP_HH
