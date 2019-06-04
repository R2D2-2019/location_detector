#pragma once

#include <gga.hpp>
#include <frame_types.hpp>
#include <usart_connection.hpp>
#include <nmea_parser.hpp>

namespace r2d2::location {
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
    class uart_nmea_c {
    private:
        std::array<uint8_t, 100> gga_sentence = {};
        size_t gga_index = 0;
        r2d2::usart::usart_connection_c &usart;
        gga_s last_result; // last result, because we may want to optimize this
                           // by only updating the location after a certain
                           // amount of time.
        /// \brief
        /// Function that creates a frame_coordinate_s struct from a gga_s
        /// struct.
        frame_coordinate_s compress(const gga_s &source);

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

        nmea_parser_c parser;

    public:
        /// \brief
        /// Function that returns the current location.
        /// \details
        /// get_location first waits for a NMEA GGA sentence on the uart port,
        /// which it then parses. The result of the parse is returned.
        frame_coordinate_s get_location();

        /// \brief
        /// uart_nmea_c contructor
        /// \param GNSS_uart_port uart port to be used to communicate with the
        /// GNSS module.
        uart_nmea_c(r2d2::usart::usart_connection_c &usart_port);
    };

} // namespace r2d2::location
