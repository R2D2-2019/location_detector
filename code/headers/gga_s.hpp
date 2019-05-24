#pragma once

#include <hwlib.hpp>

namespace r2d2::location_detector{

    struct gga_s
    {
        uint32_t time;
        float latitude;
        char noth_south_hemisphere;
        float longitude;
        char east_west_hemisphere;
        uint8_t fix_quality;
        uint8_t satellites_tracked;
        float horizontal_dilution;
        float altitude;
        char altidude_measurement;
        float geoid_height;
        char geoid_height_measurement;
    };
    

} // namespace r2d2::location_detector
