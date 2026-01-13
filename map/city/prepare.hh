#ifndef RT_PREPARE_HH
#define RT_PREPARE_HH

#include <random>
#include <vector>

#include "geometry/shapes.hh"
#include "geometry/point.hh"

enum RoadShape : int { Terminal, Across, Y, TwoLines };

std::vector<geo::Shape> create_road(RoadShape road_shape, float area_size, geo::Point* center, Random& random);

#endif //RT_PREPARE_HH
