#ifndef RT_MAP_HH
#define RT_MAP_HH

#include <random>

#include "minimap.hh"

namespace map {

class Map {
public:
    explicit Map(MapConfig const& map_config);

    [[nodiscard]] Minimap const& minimap() const { return minimap_; }


private:
    std::mt19937 rng_;
    Minimap minimap_;

public:
    const size_t w, h;
};

}

#endif //RT_MAP_HH
