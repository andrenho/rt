#include "city.hh"

#include <algorithm>
#include <ranges>
namespace ranges = std::ranges;

namespace city {

static std::vector<geo::Shape> create_poisson_disks(CityConfig const& cfg)
{
    auto max_building_sz = ranges::max_element(cfg.buildings, [](BuildingConfig const& b, BuildingConfig const& c) { return b.size < c.size; });

    auto points = geo::Point::poisson(
            geo::Bounds({ cfg.center.x - cfg.max_size/2.f, cfg.center.y - cfg.max_size/2.f }, { cfg.center.x + cfg.max_size/2.f, cfg.center.y + cfg.max_size/2.f}),
            max_building_sz->size, cfg.seed);

    auto view = points | std::views::transform([&max_building_sz](geo::Point const& p) {
        return geo::Shape::Circle(p, max_building_sz->size);
    });

    return { view.begin(), view.end() };
}

City generate_city(CityConfig const& cfg)
{
    return {
        .original_poisson_disks = create_poisson_disks(cfg)
    };
}

}