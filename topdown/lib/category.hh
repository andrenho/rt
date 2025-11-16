#ifndef TOPDOWN_CATEGORY_HH
#define TOPDOWN_CATEGORY_HH

#include <cstdint>

enum class Category : uint64_t {
    Solid    = 0b0000001,
    Sensor   = 0b0000010,
    Wheel    = 0b0000100,
    Dynamic  = 0b0001000,
    Missile  = 0b0010000,
};

#endif //TOPDOWN_CATEGORY_HH
