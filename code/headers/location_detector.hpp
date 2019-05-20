#pragma once

#include "hwlib.hpp"
#include <base_module.hpp>

namespace r2d2::location_detector {

    class module_c : public base_module_c {
    public:
        void process();

    protected:
        virtual frame_coordinate_s get_location();
        frame_coordinate_s compress(float longitude, float latitude, bool north, bool east, int16_t altitude);
    };

} // namespace r2d2::location_detector