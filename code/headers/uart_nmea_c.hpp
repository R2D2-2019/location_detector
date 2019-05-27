#pragma once

#include "location_detector.hpp"

namespace r2d2::location_detector {

    /// \brief
    /// Class that can interface with any GNSS module that outputs NMEA gga
    /// sentences on an uart/usart serial port. \details This class uses an
    /// uart/usart port to inteface with a GNSS module to determine its
    /// location. It inherits from module_c which means it has a progress()
    /// function. In this function the class will check if it received any
    /// messages on the CAN-bus. If it received a location request it will send
    /// a frame containing information about the location on the CAN-bus. This
    /// class will only work if the GNSS module outputs NMEA gga messages.
    class uart_nmea_c : public module_c {
    public:
        /// \brief
        /// uart_nmea_c base constructor.
        uart_nmea_c(){};
    };

} // namespace r2d2::location_detector
