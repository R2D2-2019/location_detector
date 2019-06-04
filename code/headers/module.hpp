#pragma once

#include <base_module.hpp>
#include <hwlib.hpp>
#include <uart_nmea.hpp>

namespace r2d2::location {

    /// \brief
    /// This class responds on a frame_coordinate_s request with a frame that
    /// has the location
    class module_c : public base_module_c {
    private:
        r2d2::location::uart_nmea_c &nmea;

        /**
         * @brief bool to store if we got a request
         *
         */
        bool request_triggered = false;

    public:
        /// \brief
        /// location_detector module_c constructor.
        module_c(r2d2::base_comm_c &comm, r2d2::location::uart_nmea_c &nmea)
            : base_module_c(comm), nmea(nmea) {

            // Set up listeners
            comm.listen_for_frames({r2d2::frame_type::COORDINATE});
        }

        /// \brief
        /// The process function of the location detection module.
        /// This function checks the CAN-bus for a location request.
        /// When a location request is received the location will be determined
        /// and send in a frame_coordinate_s frame over the CAN-bus.
        void process();
    };

} // namespace r2d2::location
