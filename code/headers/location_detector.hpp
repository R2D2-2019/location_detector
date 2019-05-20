#pragma once

#include <hwlib.hpp>
#include <base_module.hpp>

namespace r2d2::location_detector {
    
    /**
    * This class provides the interface for the location_detection module.
    */
    
    class module_c : public base_module_c {
    public:
        void process();

    protected:
        /**
         * @brief Pure virtual interface function to get the location.
         */
        
        virtual frame_coordinate_s get_location() = 0;
        /**
         * @brief This method is used to manually make coordinate frames.
         *
         * @param longitude
         * @param latitude
         * @param north
         * @param south
         * @param altitude
         */
        frame_coordinate_s compress(float longitude, float latitude, bool north, bool east, int16_t altitude);
    };

} // namespace r2d2::location_detector
