#include "prepare.hh"

enum Side : int { N, W, E, S };
static constexpr float ROAD_WIDTH = 8.f;

static geo::Point random_point(Side side, Random& random, std::uniform_real_distribution<float>& positions, float area_size)
{
    switch (side) {
        case N: return { random.next_float(.3f * area_size, .7f * area_size), .0f };
        case S: return { random.next_float(.3f * area_size, .7f * area_size), area_size };
        case W: return { .0f, random.next_float(.3f * area_size, .7f * area_size) };
        case E: return { area_size, random.next_float(.3f * area_size, .7f * area_size) };
    }

    abort();
}

static geo::Point point_along_the_line(geo::Point p1, geo::Point p2, float percentage)
{
    return { p1.x + percentage * (p2.x - p1.x), p1.y + percentage * (p2.y - p1.y) };
}

std::vector<geo::Shape> create_road(RoadShape road_shape, float area_size, geo::Point* center, Random& random)
{
    std::vector<geo::Shape> r;

    std::uniform_int_distribution<int> sides(0, 3);
    Side side1 = (Side) random.next_uint(3);
    Side side2; do { side2 = (Side) random.next_uint(3); } while (side2 == side1);
    Side side3; do { side3 = (Side) random.next_uint(3); } while (side3 == side1 || side3 == side2);

    std::uniform_real_distribution<float> positions(.3f * area_size, .7f * area_size);
    geo::Point p1 = random_point(side1, random, positions, area_size);
    geo::Point p2 = random_point(side2, random, positions, area_size);
    geo::Point p3 = random_point(side3, random, positions, area_size);

    std::uniform_real_distribution<float> percentages(.2f, .8f);

    switch (road_shape) {
        case Terminal:
            *center = geo::Point(random.next_float(.3f * area_size, .7f * area_size), random.next_float(.3f * area_size, .7f * area_size));
            r.push_back(geo::Shape::Capsule(*center, p1, ROAD_WIDTH));
            break;
        case Across:
            *center = point_along_the_line(p1, p2, random.next_float(.2f, .8f));
            r.push_back(geo::Shape::Capsule(p1, p2, ROAD_WIDTH));
            break;
        case Y: {
            *center = point_along_the_line(p1, p2, random.next_float(.2f, .8f));
            r.push_back(geo::Shape::Capsule(p1, p2, ROAD_WIDTH));
            r.push_back(geo::Shape::Capsule(*center, p3, ROAD_WIDTH));
            break;
        }
        case TwoLines: {
            geo::Point pp1 = random_point(N, random, positions, area_size),
                       pp2 = random_point(S, random, positions, area_size),
                       pp3 = random_point(W, random, positions, area_size),
                       pp4 = random_point(E, random, positions, area_size);
            r.push_back(geo::Shape::Capsule(pp1, pp2, ROAD_WIDTH));
            r.push_back(geo::Shape::Capsule(pp3, pp4, ROAD_WIDTH));
            geo::Point::segment_intersection(pp1, pp2, pp3, pp4, center);
            break;
        }
    }

    return r;
}

