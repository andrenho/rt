#ifndef MAP_HH
#define MAP_HH

#include "geometry/point.hh"

namespace map {

struct MapConfig {
    int map_w;
    int map_h;
};

class Map {
public:
    void initialize(MapConfig const& cfg);

    [[nodiscard]] geo::Size size() const { return { (float) cfg_.map_w, (float) cfg_.map_h }; }

private:
    MapConfig cfg_;
};

} // map

#endif //MAP_HH
