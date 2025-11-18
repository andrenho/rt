#ifndef TOPDOWN_VEHICLECONFIG_HH
#define TOPDOWN_VEHICLECONFIG_HH

#include <cstdint>
#include <optional>

namespace topdown {

struct VehicleConfig {
    float   h, w;
    float   acceleration;
    float   wheelbase;
    float   skid;
    float   drag;
    float   steering;
    bool    fixed_acceleration;
    uint8_t n_wheels;
};

}

#endif //TOPDOWN_VEHICLECONFIG_HH
