#ifndef FEATURES_HH
#define FEATURES_HH

#include "geo/shapes.hh"

namespace map {

struct Terrain {
    enum class Type { Dirt };
    Type  type;
    Shape shape;
};

}

#endif //FEATURES_HH
