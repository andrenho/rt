#include "map.hh"

#include <random>

#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi.h"

namespace map {

void Map::initialize(MapConfig const& cfg)
{
    cfg_ = cfg;

    rng_ = std::mt19937(cfg.seed);

    polygon_points.clear();
    polygons.clear();

    generate_points();
    generate_polygons();
}

void Map::generate_points()
{
    // generate points
    int w = cfg_.map_w / cfg_.point_density;
    int h = cfg_.map_h / cfg_.point_density;
    for (int x = 1; x < w; ++x)
        for (int y = 1; y < h; ++y)
            polygon_points.emplace_back(x * cfg_.point_density, y * cfg_.point_density);

    // add randomness
    std::uniform_real_distribution<float> distances(0.0, ((float) cfg_.point_density) * cfg_.point_randomness);
    std::uniform_real_distribution<float> angles(0.0, 2.0);
    for (auto& p: polygon_points) {
        p.x += distances(rng_) * (float) cos(angles(rng_));
        p.y += distances(rng_) * (float) sin(angles(rng_));
    }
}

void Map::generate_polygons()
{
    jcv_diagram diagram {};
    std::vector<jcv_point> jcv_points; jcv_points.reserve(polygon_points.size());
    for (auto const& p: polygon_points)
        jcv_points.emplace_back(p.x, p.y);

    jcv_diagram_generate((int) jcv_points.size(), jcv_points.data(), nullptr, nullptr, &diagram);

    const jcv_site* sites = jcv_diagram_get_sites(&diagram);
    for(int i = 0; i < diagram.numsites; ++i) {
        geo::Polygon polygon;

        const jcv_site* site = &sites[i];

        const jcv_graphedge* e = site->edges;
        while (e) {
            polygon.emplace_back(e->pos[0].x, e->pos[0].y);
            e = e->next;
        }

        polygons.emplace_back(std::move(polygon));
    }

    jcv_diagram_free(&diagram);
}

} // map
