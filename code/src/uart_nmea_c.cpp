#include <uart_nmea_c.hpp>
#include <cstdint>
#include <cmath>
#include <string.hpp>

namespace r2d2::location {
    uart_nmea_c::uart_nmea_c(r2d2::usart::usart_connection_c &usart_port)
        : usart(usart_port) {
    }

    frame_coordinate_s uart_nmea_c::compress(const gga_s &source) {
        return compress(source.longitude, source.latitude,
                        (source.north_south_hemisphere == 'N'),
                        (source.east_west_hemisphere == 'E'), source.altitude);
    }

    frame_coordinate_s uart_nmea_c::get_location() {
        enum get_location_state {
            wait_for_start,
            get_message_id,
            check_message_id,
            get_sentence
        };

        get_location_state state = wait_for_start;
        char uart_input = 0;

        while (true) {
            switch (state) {
            // wait for a $ sign
            case wait_for_start: {
                uint8_t c = usart.receive();

                if (c == 36) {
                    gga_sentence[gga_index++] = '$';
                    state = get_message_id;
                }

                break;
            }


            // get the message ID
            case get_message_id:
                if (usart.available()) {
                    gga_sentence[gga_index++] = usart.getc();
                    if (gga_sentence[gga_index - 1] == ',') {
                        // a comma indicates the end of the message id
                        state = check_message_id;

                        // the message ID, together with the '$' at the start
                        // and ',' at the end, has a length of 7 characters. If
                        // the ID is longer, it cannot be the ID we are looking
                        // for.
                    } else if (gga_index > 7) {
                        gga_index = 0;
                        state = wait_for_start;
                    }
                }
                break;

            // check if the message ID is equal to GGA
            case check_message_id:
                if (gga_sentence[3] == 'G' && gga_sentence[4] == 'G' && gga_sentence[5] == 'A') {
                    // if GGA, then send the characters to a string
                    state = get_sentence;
                } else { // if not GGA, clear the string and wait for a $ sign
                    gga_index = 0;
                    state = wait_for_start;
                }
                break;

            case get_sentence: {
                if (usart.available()) {
                    uart_input = usart.receive();

                    if (uart_input == '\r' || uart_input == '\n') {
                        last_result = parser.parse_nmea(gga_sentence.begin(), gga_index);
                        return compress(last_result);
                    } else {
                        gga_sentence[gga_index++] = uart_input;

                        // if the maximum number of characters has been reached,
                        // clear the string and wait for a $ sign
                        if (gga_index >= 100) {
                            gga_index = 0;
                            state = wait_for_start;
                        }
                    }
                }
                break;
            }

            default:
                break;
            }
        }
    }

    frame_coordinate_s uart_nmea_c::compress(float longitude, float latitude,
                                          bool north, bool east,
                                          int16_t altitude) {

        frame_coordinate_s frame;

        frame.altitude = altitude;

        frame.long_deg = static_cast<uint8_t>(longitude / 100);
        frame.long_sec = static_cast<uint8_t>(longitude - frame.long_deg * 100);
        frame.long_thousandth_sec = static_cast<uint16_t>((longitude - static_cast<int>(longitude)) * 1000);
        frame.east_west_hemisphere = east;

        frame.lat_deg = static_cast<uint8_t>(latitude / 100);
        frame.lat_sec = static_cast<uint8_t>(latitude - frame.lat_deg * 100);
        frame.lat_thousandth_sec = static_cast<uint16_t>((latitude - static_cast<int>(latitude)) * 1000);
        frame.north_south_hemisphere = north;
        return frame;
    }

} // namespace r2d2::location
