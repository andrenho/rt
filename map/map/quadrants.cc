#include "quadrants.hh"

namespace map {

Quadrants generate_quadrants(PhysicalMap const& pmap, size_t size)
{
    Quadrants quadrants;

    // create quadrants and bounds
    for (int x = 0; x < pmap.w; x += (int) size) {
        for (int y = 0; y < pmap.h; y += (int) size) {
            geo::Bounds bounds { { x - size, y - size }, { x + (size * 2), y + (size * 2) } };

            PhysicalMap pp;
            pp.w = pp.h = (int) size;

            // water
            for (auto const& water: pmap.water)
                if (water.aabb_intersects(bounds))
                    pp.water.push_back(water);

            // terrains
            for (auto const& terrain: pmap.terrains)
                if (terrain.shape.aabb_intersects(bounds))
                    pp.terrains.push_back(terrain);

            // water
            for (auto const& t: pmap.unpassable_terrains)
                if (t.aabb_intersects(bounds))
                    pp.unpassable_terrains.push_back(t);

            // water
            for (auto const& t: pmap.roads)
                if (t.aabb_intersects(bounds))
                    pp.roads.push_back(t);

            quadrants[{ x / size, y / size }] = std::move(pp);
        }
    }

    return quadrants;
}

}