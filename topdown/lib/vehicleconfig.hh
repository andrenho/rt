#ifndef TOPDOWN_VEHICLECONFIG_HH
#define TOPDOWN_VEHICLECONFIG_HH

namespace topdown {

struct VehicleConfig {
    float h, w;
    float acceleration;
    float wheelbase;
};

namespace vehicle {
    constexpr VehicleConfig Car = {
        .h = 5.f,
        .w = 2.f,
        .acceleration = 10.f,
        .wheelbase = 3.f,
    };
}

}

#endif //TOPDOWN_VEHICLECONFIG_HH
