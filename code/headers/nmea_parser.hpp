#pragma once

#include <cstdint>
#include <gga.hpp>

namespace r2d2::location {
    class nmea_parser_c {
    protected:
        enum class GGA : uint8_t {
            sentence,
            time,
            latitude,
            north_south_hemisphere,
            longitude,
            east_west_hemisphere,
            fix_quality,
            satellites_tracked,
            horizontal_dilution,
            altitude,
            altitude_measurement,
            geoid_height,
            geoid_height_measurement
        };

    public:
        /**
         * @brief converts a gga string to a gga_struct
         *
         * @param gps_message
         * @param length
         * @return gga_s
         */
        static gga_s parse_nmea(const uint8_t *gps_message, const size_t length);
    };
} // namespace r2d2::location