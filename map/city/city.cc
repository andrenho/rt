#include "city.hh"

#include <algorithm>
#include <ranges>
namespace ranges = std::ranges;

namespace city {

static std::vector<geo::Shape> create_poisson_disks(CityConfig const& cfg, float max_building_sz)
{
    auto points = geo::Point::poisson(
            geo::Shape::Box({ cfg.center.x - cfg.max_size/2.f, cfg.center.y - cfg.max_size/2.f }, { cfg.center.x + cfg.max_size/2.f, cfg.center.y + cfg.max_size/2.f}),
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

static std::vector<City::Building> create_buildings(std::vector<BuildingConfig> const& buildings,
    std::vector<geo::Shape> const& poisson_disks, std::variant<float, geo::Point> city_direction, float angle_variation, std::mt19937& rng)
{
    std::vector<BuildingConfig> bs;
    for (auto const& b: buildings) {
        for (size_t i = 0; i < b.count; ++i) {
            bs.emplace_back(b.w, b.h, b.door_position, 1);
        }
    }

    std::uniform_real_distribution dist(-angle_variation, angle_variation);

    std::vector<City::Building> r;
    for (size_t i = 0; i < std::min(bs.size(), poisson_disks.size()); ++i) {
        BuildingConfig const& config = bs[i];
        if (config.door_position < 0.f || config.door_position >= 1.f)
            throw std::runtime_error("'door_position' needs to be between 0 and 1");

        float angle = std::visit(overloaded {
            [&](float a) { return a; },
            [&](geo::Point const& center) { return center.angle(poisson_disks.at(i).center()); }
        }, city_direction);

        auto box = geo::Shape::Box(poisson_disks.at(i).center() - geo::Point(config.h / 2.f, config.w / 2.f), { config.h, config.w }, angle + dist(rng));
        auto door_line = geo::shape::polygon_lines(std::get<geo::shape::Polygon>(box.for_visit())).at(0);
        auto door_point = geo::Point(
            door_line.p1.x + config.door_position * (door_line.p2.x - door_line.p1.x),
            door_line.p1.y + config.door_position * (door_line.p2.y - door_line.p1.y)
        );

        r.emplace_back(box, door_point);
    }
    return r;
}

City generate_city(CityConfig const& cfg)
{
    auto max_building_sz = ranges::max_element(cfg.buildings, [](BuildingConfig const& b, BuildingConfig const& c) { return b.size() < c.size(); })->size() / 2.f;
    auto total_building_count = std::accumulate(cfg.buildings.begin(), cfg.buildings.end(), 0, [](int s, BuildingConfig const& b) { return b.count + s; });

    City city;
    city.original_poisson_disks = create_poisson_disks(cfg, max_building_sz);
    auto poisson_disks = remove_obstacle_overlaps(city.original_poisson_disks, cfg);

    auto points = poisson_disks | std::views::transform([&](geo::Shape const& disk) { return disk.center(); });
    auto closest_points = geo::Point::closest_points({ points.begin(), points.end() }, cfg.center, total_building_count);

    auto circles = closest_points | std::views::transform([&](geo::Point const& p) { return geo::Shape::Circle(p, max_building_sz); });
    city.poisson_disks = { circles.begin(), circles.end() };

    std::mt19937 rng(cfg.seed);
    city.buildings = create_buildings(cfg.buildings, city.poisson_disks, cfg.city_direction, cfg.angle_variation, rng);

    return city;
}

}