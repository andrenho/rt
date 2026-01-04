#include "city.hh"

#include <algorithm>
#include <ranges>
namespace ranges = std::ranges;

namespace city {

static std::vector<geo::Shape> create_poisson_disks(CityConfig const& cfg, float max_building_sz)
{
    auto points = geo::Point::poisson(
            geo::Bounds({ cfg.center.x - cfg.max_size/2.f, cfg.center.y - cfg.max_size/2.f }, { cfg.center.x + cfg.max_size/2.f, cfg.center.y + cfg.max_size/2.f}),
            max_building_sz + 2.f, cfg.seed);

    auto view = points | std::views::transform([&max_building_sz](geo::Point const& p) {
        return geo::Shape::Circle(p, max_building_sz);
    });

    return { view.begin(), view.end() };
}

static std::vector<geo::Shape> remove_obstacle_overlaps(std::vector<geo::Shape> const& disks, CityConfig const& cfg)
{
    auto new_disks = disks | std::views::filter([&](geo::Shape const& disk) {
        return ranges::all_of(cfg.obstacles, [&](geo::Shape const& obstacle) { return !obstacle.expand(2.f).intersects(disk); });
    });
    return { new_disks.begin(), new_disks.end() };
}

City generate_city(CityConfig const& cfg)
{
    auto max_building_sz = ranges::max_element(cfg.buildings, [](BuildingConfig const& b, BuildingConfig const& c) { return b.size < c.size; })->size / 2.f;
    auto total_building_count = std::accumulate(cfg.buildings.begin(), cfg.buildings.end(), 0, [](int s, BuildingConfig const& b) { return b.count + s; });

    City city;
    city.original_poisson_disks = create_poisson_disks(cfg, max_building_sz);
    auto poisson_disks = remove_obstacle_overlaps(city.original_poisson_disks, cfg);

    auto points = poisson_disks | std::views::transform([&](geo::Shape const& disk) { return disk.center(); });
    auto closest_points = geo::Point::closest_points({ points.begin(), points.end() }, cfg.center, total_building_count);

    auto circles = closest_points | std::views::transform([&](geo::Point const& p) { return geo::Shape::Circle(p, max_building_sz); });
    city.poisson_disks = { circles.begin(), circles.end() };

    return city;
}

}