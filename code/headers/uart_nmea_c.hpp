#pragma once

#include <gga.hpp>
#include <frame_types.hpp>
#include <usart_connection.hpp>
#include <nmea_parser.hpp>
#include <nmea_listener.hpp>

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
        /**
         * @brief Nmea parser for parsing all the strings
         * 
         */
        nmea_parser_c parser;

        /**
         * @brief usart listener for getting a string of nmea data.
         * 
         */
        nmea_listener listener;

    public:
        /// \brief
        /// uart_nmea_c contructor
        /// \param GNSS_uart_port uart port to be used to communicate with the
        /// GNSS module.
        uart_nmea_c(r2d2::usart::usart_connection_c &usart_port);

        /// \brief
        /// Function that returns the current location.
        /// \details
        /// get_location first waits for a NMEA GGA sentence on the uart port,
        /// which it then parses. The result of the parse is returned.
        gga_s get_location();

        /// \brief
        /// Function that creates a frame_coordinate_s struct from a gga_s
        /// struct.
        frame_coordinate_s gga_to_frame(const gga_s &source);

        /**
         * \brief This method is used to manually make coordinate frames.
         *
         * \param longitude
         * \param latitude
         * \param north
         * \param south
         * \param altitude
         */
        frame_coordinate_s coordinates_to_frame(float longitude, float latitude, bool north,
                                    bool east, int16_t altitude);

    };

} // namespace r2d2::location
