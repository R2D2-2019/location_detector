#pragma once

#include <base_module.hpp>
#include <hwlib.hpp>
#include <uart_nmea_c.hpp>

namespace r2d2::location {

    /// \brief
    /// This class provides the interface for the location_detection module.
    /// \details
    /// TODO: what is the input (frame) of this module?
    /// what is the output (frame) of this module?
    class module_c : public base_module_c {
    private:
       r2d2::location::uart_nmea_c &nmea;

    public:
        /// \brief
        /// location_detector module_c constructor.
        module_c(r2d2::base_comm_c &comm, r2d2::location::uart_nmea_c &nmea) : base_module_c(comm), nmea(nmea) {
            // TODO: what frames are we listening for?
        }

        /// \brief
        /// The process function of the location detection module.
        /// This function checks the CAN-bus for a location request.
        /// When a location request is received the location will be determined
        /// and send in a frame_coordinate_s frame over the CAN-bus.
        void process();

    protected:

    };

} // namespace r2d2::location
