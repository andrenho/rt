#ifndef TOPDOWN_VEHICLECONFIG_HH
#define TOPDOWN_VEHICLECONFIG_HH

namespace topdown {

struct VehicleConfig {
    unsigned int h, w;
};

namespace vehicle {
    constexpr VehicleConfig Car = { .h = 5, .w = 2 };
}

}

#endif //TOPDOWN_VEHICLECONFIG_HH
