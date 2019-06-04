#pragma once

#include <hwlib.hpp>

namespace r2d2::location {
    /**
     * @brief this struct contains essential fix data which provide 3D location
     * and accuracy data
     *
     * https://www.gpsinformation.org/dale/nmea.htm#GGA
     *
     */
    struct gga_s {
        uint32_t time;
        float latitude;
        char north_south_hemisphere;
        float longitude;
        char east_west_hemisphere;
        uint8_t fix_quality;
        uint8_t satellites_tracked;
        float horizontal_dilution;
        float altitude;
        char altitude_measurement;
        float geoid_height;
        char geoid_height_measurement;
    };

} // namespace r2d2::location
