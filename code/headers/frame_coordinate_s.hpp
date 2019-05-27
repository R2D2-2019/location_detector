#pragma once

#include <hwlib.hpp>

namespace r2d2::location_detector {

    struct frame_coordinate_s {
        uint8_t latitude_degrees;
        uint8_t latitude_minutes;
        uint8_t latitude_seconds;
        uint16_t latitude_thousandth_second;

        uint8_t longitude_degrees;
        uint8_t longitude_minutes;
        uint8_t longitude_seconds;
        uint16_t longitude_thousandth_second;

        char north_south_hemisphere;
        char east_west_hemisphere;

        uint16_t altitude;
    };

} // namespace r2d2::location_detector