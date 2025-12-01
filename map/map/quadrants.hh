#ifndef RT_QUADRANTS_HH
#define RT_QUADRANTS_HH

#include "physicalmap.hh"

#include <unordered_map>
#include <utility>

namespace map {

using Quadrant = std::pair<int, int>;

using Quadrants = std::unordered_map<Quadrant, PhysicalMap>;

Quadrants generate_quadrants(PhysicalMap const& pmap, size_t size);

}

namespace std {
template <>
struct hash<map::Quadrant> {
    std::size_t operator()(const map::Quadrant& q) const {
        std::size_t seed = 0;
        seed ^= std::hash<int>{}(q.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>{}(q.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};
}

#endif //RT_QUADRANTS_HH
