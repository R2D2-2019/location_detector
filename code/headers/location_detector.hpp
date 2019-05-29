#pragma once

#include <base_module.hpp>
#include <hwlib.hpp>

namespace r2d2::location_detector {

    /// \brief
    /// This class provides the interface for the location_detection module.
    /// \details
    /// TODO: what is the input (frame) of this module?
    /// what is the output (frame) of this module?
    class module_c : public base_module_c {
    public:
        /// \brief
        /// location_detector module_c constructor.
        module_c(base_comm_c &comm) : base_module_c(comm) {
            // TODO: what frames are we listening for?
        }

        /// \brief
        /// The process function of the location detection module.
        /// This function checks the CAN-bus for a location request.
        /// When a location request is received the location will be determined
        /// and send in a frame_coordinate_s frame over the CAN-bus.
        void process();

    protected:
        /**
         * \brief Pure virtual interface function to get the location.
         */

        virtual frame_coordinate_s get_location() = 0;
        /**
         * \brief This method is used to manually make coordinate frames.
         *
         * \param longitude
         * \param latitude
         * \param north
         * \param south
         * \param altitude
         */
        frame_coordinate_s compress(float longitude, float latitude, bool north,
                                    bool east, int16_t altitude);
    };

} // namespace r2d2::location_detector
