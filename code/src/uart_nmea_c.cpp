#include "uart_nmea_c.hpp"

namespace r2d2::location_detector {

    uart_nmea_c::uart_nmea_c(usart::usart_connection_c &GNSS_uart_port)
        : gga_sentence(), GNSS_uart_port(GNSS_uart_port) {
    }

    frame_coordinate_s uart_nmea_c::compress(const gga_s &source) {
        return compress(source.longitude, source.latitude,
                        (source.north_south_hemisphere == "N"),
                        (source.east_west_hemisphere == "E"), source.altitude);
    }

    frame_coordinate_s uart_nmea_c::get_location() {
        enum get_location_state {
            clear_buffer,
            wait_for_start,
            get_message_id,
            check_message_id,
            get_sentence
        };
        get_location_state state = clear_buffer;
        char uart_input = 0;

        while (true) {
            switch (state) {
            // clear the uart buffer
            case clear_buffer:
                while (GNSS_uart_port.char_available()) {
                    GNSS_uart_port.receive()
                }
                state = wait_for_start;
                break;

            // wait for a $ sign
            case wait_for_start:
                if (GNSS_uart_port.getc() == '$') {
                    gga_sentence.append('$');
                    state = get_message_id;
                }
                break;

            // get the message ID
            case get_message_id:
                if (GNSS_uart_port.char_available()) {
                    gga_sentence.append(GNSS_uart_port.getc());
                    if (gga_sentence[gga_sentence.length() - 1] == ',') {
                        // a comma indicates the end of the message id
                        state = check_message_id;

                        // the message ID, together with the '$' at the start
                        // and ',' at the end, has a length of 7 characters. If
                        // the ID is longer, it cannot be the ID we are looking
                        // for.
                    } else if (gga_sentence.length() > 7) {
                        gga_sentence.clear();
                        state = clear_buffer;
                    }
                }
                break;

            // check if the message ID is equal to GGA
            case check_message_id:
                if (gga_sentence.find_at("GGA", 2)) {
                    // if GGA, then send the characters to a string
                    state = get_sentence;
                } else { // if not GGA, clear the string and wait for a $ sign
                    gga_sentence.clear();
                    state = clear_buffer;
                }
                break;

            case get_sentence:
                if (GNSS_uart_port.char_available()) {
                    uart_input = GNSS_uart_port.getc();

                    // if a $ is encountered, clear the string and get the
                    // message ID.
                    if (uart_input == '$') {
                        gga_sentence.clear();
                        gga_sentence.append('$');
                        state = get_message_id;

                        // if the character is a carriage return (ascii code
                        // 13), safe the string and parse it.
                    } else if (uart_input == 13) {
                        last_result = parse_nmea(gga_sentence);
                        return compress(last_result);
                    } else {
                        gga_sentence.append(uart_input);

                        // if the maximum number of characters has been reached,
                        // clear the string and wait for a $ sign
                        if (gga_sentence.length() >= 100) {
                            gga_sentence.clear();
                            state = wait_for_start;
                        }
                    }
                }
                break;

            default:
                break;
            }
        }

    }

    gga_s uart_nmea_c::parse_nmea(hwlib::string<0> nmea_sentence){
        // TODO: pull from the 'feature-parse_nmea_function' branch
    }

} // namespace r2d2::location_detector
