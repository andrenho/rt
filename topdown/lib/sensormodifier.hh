#ifndef TOPDOWN_SENSORMODIFIER_HH
#define TOPDOWN_SENSORMODIFIER_HH

namespace topdown {

struct SensorModifier {
    float acceleration;
    float skid;
};

namespace terrain {

    constexpr SensorModifier Asphalt = {
        .acceleration = 1.f,
        .skid = 0.f,
    };

    constexpr SensorModifier Dirt = {
        .acceleration = .7f,
        .skid = .5f,
    };

    constexpr SensorModifier Ice = {
        .acceleration = .6f,
        .skid = 1.2f,
    };
}

}

#endif //TOPDOWN_SENSORMODIFIER_HH
