#pragma once

#include <frame_types.hpp>
#include <gga.hpp>
#include <nmea_listener.hpp>
#include <nmea_parser.hpp>
#include <usart_connection.hpp>

namespace r2d2::location {
    /// \brief
    /// Class that can interface with any GNSS module that outputs NMEA gga
    /// sentences on an uart/usart serial port.
    /// \details
    /// This class uses an uart/usart port to inteface with a GNSS
    /// module to determine its location.  This class will only work if the GNSS
    /// module outputs NMEA gga messages.
    class uart_nmea_c {
    private:
        /**
         * @brief Nmea parser for parsing all the strings
         *
         */
        nmea_parser_c parser;

        /**
         * @brief usart listener for getting a string of nmea data.
         *
         */
        nmea_listener_c listener;

    public:
        /// \brief
        /// uart_nmea_c contructor
        /// \param GNSS_uart_port uart port to be used to communicate with the
        /// GNSS module.
        uart_nmea_c(r2d2::usart::usart_connection_c &usart_port);

        /// \brief
        /// Function that returns the current location.
        /// \details
        /// when no new location is available this will return a gga_s struct
        /// with a fix_quality on 0 for an invalid fix.
        gga_s get_location();

        /// \brief
        /// Function that creates a frame_coordinate_s struct from a gga_s
        /// struct.
        frame_coordinate_s gga_to_frame(const gga_s &source);

        /**
         * \brief This method is used to create a coordinate frames.
         *
         * \param longitude
         * \param latitude
         * \param north
         * \param south
         * \param altitude
         */
        frame_coordinate_s coordinates_to_frame(degrees longitude, degrees latitude,
                                                bool north, bool east,
                                                int16_t altitude);
    };

} // namespace r2d2::location
