#pragma once

#include "gga_s.hpp"
#include "location_detector.hpp"
#include <usart_connection.hpp>

namespace r2d2::location_detector {

    /// \brief
    /// Class that can interface with any GNSS module that outputs NMEA gga
    /// sentences on an uart/usart serial port.
    /// \details
    /// This class uses an uart/usart port to inteface with a GNSS
    /// module to determine its location. It inherits from module_c which means
    /// it has a progress() function. In this function the class will check if
    /// it received any messages on the CAN-bus. If it received a location
    /// request it will send a frame containing information about the location
    /// on the CAN-bus. This class will only work if the GNSS module outputs
    /// NMEA gga messages.
    class uart_nmea_c : public module_c {
    private:
        hwlib::string<100> gga_sentence;
        usart::usart_connection_c &GNSS_uart_port;
        gga_s last_result; // last result, because we may want to optimize this
                           // by only updating the location after a certain
                           // amount of time.

        frame_coordinate_s compress(const gga_s &source);
        gga_s parse_nmea(hwlib::string<0> nmea_sentence);

    protected:
        frame_coordinate_s get_location() override;

    public:
        /// \brief
        /// uart_nmea_c contructor
        /// \param GNSS_uart_port uart port to be used to communicate with the
        /// GNSS module.
        uart_nmea_c(usart::usart_connection_c &GNSS_uart_port);
    };

} // namespace r2d2::location_detector
