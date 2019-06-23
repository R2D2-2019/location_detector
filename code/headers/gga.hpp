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
        // UTC time of fix
        uint32_t time;

        // Latitude/ longitude
        uint32_t latitude;
        uint32_t longitude;

        // Horizontal dilution of precision. The lower the number the better.
        // This number is lower if the satellites are more spread out.
        // https://the5krunner.com/2018/07/18/gps-accuracy-hdop-pdop-gdop-multipath-the-atmosphere/
        float horizontal_dilution;

        // Altitude above sea level. The precision of the altitude is always
        // lower than the lat/long. The precision of the altitude is not in the
        // gga message.
        float altitude;

        // altitude using the WGS-84 ellipsoid
        float geoid_height;

        // the age in seconds since the last update from the diff reference
        // station.
        uint32_t diff_age;

        // the id of the diff reference station
        uint16_t diff_id;

        // variable for how reliable the data is.
        // 0 = invalid
        // 1 = GPS fix (SPS)
        // 2 = DGPS fix, OmniSTAR VBS
        // 3 = PPS fix
        // 4 = Real Time Kinematic, fixed integers
        // 5 = Float RTK, float integers, OmniSTAR XP/HP or Location RTK
        // 6 = estimated (dead reckoning) (2.3 feature)
        // 7 = Manual input mode
        // 8 = Simulation mode
        uint8_t fix_quality;

        // variable that stores how much gps satellites we are tracking.
        uint8_t satellites_tracked;

        // bool for what side of the hemispere the location is
        bool is_north_hemisphere;
        bool is_east_hemisphere;

        // units for altitude and geoid separation
        char altitude_unit;
        char geoid_height_unit;
    };

} // namespace r2d2::location
