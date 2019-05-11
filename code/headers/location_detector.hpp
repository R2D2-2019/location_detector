#pragma once

#include "hwlib.hpp"
#include <base_module.hpp>

namespace r2d2::location_detector {

    class location_detection_c {
    public:
        void process();

    protected:
        virtual frame_coordinate_s get_location();
        GGA parse_nmea(hwlib::string);
        frame_coordinate_s compress(GGA);
        frame_coordinate_s compress(uint32_t longitude, uint32_t latitude, bool north, bool east, int16_t altitude);
    };

} // namespace r2d2::location_detector