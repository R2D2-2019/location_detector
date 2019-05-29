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
        /// \brief
        /// Function that creates a frame_coordinate_s struct from a gga_s
        /// struct.
        frame_coordinate_s compress(const gga_s &source);
        /// \brief
        /// returns time as an integer
        /// \details
        /// This function return time as an integer by converting it from a
        /// hwlib::string
        uint32_t time_maker(const hwlib::string<10> &time);
        /// \brief
        /// returns either a longitude or latitude as a float
        /// \details
        /// converts a hwlib::string to a calculatable float, useable as a
        /// coordinate
        float latitude_longitude_maker(const hwlib::string<15> &coordinate);
        /// \brief
        /// returns quality of fix
        /// \details
        /// converts a hwlib::string to an integer
        uint8_t fix_maker(const hwlib::string<10> &fix);
        /// \brief
        /// returns the number of satellites tracked
        /// \details
        /// converts a hwlib::string to an integer
        uint8_t satellite_maker(const hwlib::string<10> &satellites);
        /// \brief
        /// returns the horizontal dilution
        /// \details
        /// returns a float after converting a hwlib::string
        float horizontal_dilution_maker(const hwlib::string<10> &dilution);
        /// \brief
        /// returns the altitude or geoid
        /// \details
        /// converts a hwlib::string to a float
        float altitude_geoid_maker(const hwlib::string<10> &altitude);
        /// \brief
        /// returns a location struct filled with corresponding
        /// information/datatypes \details chops up a hwlib::string and calls
        /// other 'maker' functions to fill struct.
        gga_s parse_nmea(const hwlib::string<0> &gps_message);

    protected:
        /// \brief
        /// Function that returns the current location.
        /// \details
        /// get_location first waits for a NMEA GGA sentence on the uart port,
        /// which it then parses. The result of the parse is returned.
        frame_coordinate_s get_location() override;

    public:
        /// \brief
        /// uart_nmea_c contructor
        /// \param GNSS_uart_port uart port to be used to communicate with the
        /// GNSS module.
        uart_nmea_c(usart::usart_connection_c &GNSS_uart_port);
    };

} // namespace r2d2::location_detector
