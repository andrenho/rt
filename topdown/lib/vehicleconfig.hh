#ifndef TOPDOWN_VEHICLECONFIG_HH
#define TOPDOWN_VEHICLECONFIG_HH

namespace topdown {

struct VehicleConfig {
    float h, w;
    float acceleration;
};

namespace vehicle {
    constexpr VehicleConfig Car = {
        .h = 5,
        .w = 2,
        .acceleration = 1000.f,
    };
}

}

#endif //TOPDOWN_VEHICLECONFIG_HH
