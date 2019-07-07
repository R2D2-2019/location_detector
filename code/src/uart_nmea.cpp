#include <cmath>
#include <cstdint>
#include <string.hpp>
#include <uart_nmea.hpp>

namespace r2d2::location {
    uart_nmea_c::uart_nmea_c(r2d2::usart::usart_connection_c &usart_port)
        : listener(usart_port) {
    }

    frame_coordinate_s uart_nmea_c::coordinates_to_frame(decimal_degrees longitude,
                                                         decimal_degrees latitude,
                                                         bool north, bool east,
                                                         int16_t altitude) {

        frame_coordinate_s frame;

        frame.altitude = altitude;

        frame.long_deg = longitude.degrees;
        frame.long_min = longitude.minutes;
        frame.long_tenthousandth_min = longitude.tenthousandths;
        frame.east_west_hemisphere = east;

        frame.lat_deg = latitude.degrees;
        frame.lat_min = latitude.minutes;
        frame.lat_tenthousandth_min = latitude.tenthousandths;
        frame.north_south_hemisphere = north;

        return frame;
    }

    frame_coordinate_s uart_nmea_c::gga_to_frame(const gga_s &source) {
        return coordinates_to_frame(source.longitude, source.latitude,
                                    source.is_north_hemisphere,
                                    source.is_east_hemisphere, source.altitude);
    }

    gga_s uart_nmea_c::get_location() {
        enum get_location_state { wait_for_string, check_string, parse_string };

        get_location_state state = wait_for_string;

        while (true) {
            switch (state) {
            // wait for string
            case wait_for_string: {
                listener.update_buffer();

                if (listener.nmea_available()) {
                    state = check_string;
                    break;
                }

                // when no message is received yet return a empty gga_s to
                // notice caller that no data is available yet.
                return gga_s{};
            }

            // check if the message_ID is the correct length
            case check_string: {
                // get the offset of the first ',' as
                // a comma indicates the end of the message id
                size_t offset_sep = get_offset_separator(
                    listener.get_nmea_string(), listener.length(), ',');

                if (offset_sep > 7 || offset_sep == 0) {
                    // the message ID, together with the '$' at the start
                    // and ',' at the end, has a length of 7 characters. If
                    // the ID is longer, it cannot be the ID we are looking
                    // for.
                    state = wait_for_string;

                    // mark string as read to wait for a new one
                    listener.mark_as_read();

                    break;
                }

                // parse the string as this looks like a valid nmea string
                state = parse_string;
                break;
            }

            // parse the string
            case parse_string: {
                auto gga = parser.parse_nmea(listener.get_nmea_string(),
                                             listener.length());

                state = wait_for_string;

                // mark string as read to wait for a new one
                listener.mark_as_read();

                return gga;
            }
            }
        }
    }
} // namespace r2d2::location
