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
        float longitude;
        float horizontal_dilution;
        float altitude;
        float geoid_height;        
        uint8_t fix_quality;
        uint8_t satellites_tracked;
        char north_south_hemisphere;
        char east_west_hemisphere;
        char altitude_measurement;
        char geoid_height_measurement;
    };

} // namespace r2d2::location
