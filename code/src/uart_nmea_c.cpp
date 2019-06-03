#include <uart_nmea_c.hpp>
#include <cstdint>
#include <cmath>

namespace r2d2::location_detector {
    size_t get_offset_separator(const uint8_t *gps_message, const size_t length, const uint8_t separator) {
        for(size_t i = 0; i < length; i++){
            if (gps_message[i] == separator){
                return i + 1;
            }
        }
        return 0;
    }

    int32_t atoi(const uint8_t *string, const size_t length) {
        int32_t res = 0;

        for (size_t i = 0; i < length; i++) {
            if (string[i] >= '0' && string[i] <= '9') {
                res = res * 10 + string[i] - '0';
            }
        }

        return res * (length ? (string[0] == '-' ? -1 : 1) : 1);
    }

    float atof(const uint8_t *string, const size_t length) {
        size_t offset = get_offset_separator(string, length, '.');

        return atoi(string, length) / (offset ? pow(float(10), int(length - offset) - 1) : 1);
    }

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
                        last_result = parse_nmea(gga_sentence.begin(), gga_index);
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

    gga_s uart_nmea_c::parse_nmea(const uint8_t *gps_message, const size_t length) {
        enum class GGA : uint8_t {
            sentence,
            time,
            latitude,
            north_south_hemisphere,
            longitude,
            east_west_hemisphere,
            fix_quality,
            satellites_tracked,
            horizontal_dilution,
            altitude,
            altitude_measurement,
            geoid_height,
            geoid_height_measurement
        };

        gga_s location;
        GGA GGAindex = GGA::sentence;
        for (size_t i = 0; i < length; i++){
            size_t sep_offset = get_offset_separator(gps_message + i, length - i, ',');

            switch (GGAindex){
                case GGA::sentence: {
                    // Current sentence, currently only supports GGA
                    break;
                }
                case GGA::time: {
                    location.time = atoi(gps_message + i, sep_offset);
                    break;
                }
                case GGA::latitude: {
                    location.latitude = atof(gps_message + i, sep_offset);
                    break;
                }
                case GGA::north_south_hemisphere: {
                    location.north_south_hemisphere = gps_message[i];
                    break;
                }
                case GGA::longitude: {
                    location.longitude = atof(gps_message + i, sep_offset);
                    break;
                }
                case GGA::east_west_hemisphere: {
                    location.east_west_hemisphere = gps_message[i];
                    break;
                }
                case GGA::fix_quality: {
                    location.fix_quality = atoi(gps_message + i, sep_offset);
                    break;
                }
                case GGA::satellites_tracked: {
                    location.satellites_tracked = atoi(gps_message + i, sep_offset);
                    break;
                }
                case GGA::horizontal_dilution: {
                    location.horizontal_dilution = atof(gps_message + i, sep_offset);
                    break;
                }
                case GGA::altitude: {
                    location.altitude = atof(gps_message + i, sep_offset);
                    break;
                }
                case GGA::altitude_measurement: {
                    location.altitude_measurement = gps_message[i];
                    break;
                }
                case GGA::geoid_height: {
                    location.geoid_height = atof(gps_message + i, sep_offset);
                    break;
                }
                case GGA::geoid_height_measurement: {
                    location.geoid_height_measurement = gps_message[i];
                    break;
                }
                default: {
                    return location;
                }
            }
            i += sep_offset ? sep_offset -1 : 0;
            GGAindex = static_cast<GGA>(static_cast<uint8_t>(GGAindex) + 1);
        }
        return location;
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

} // namespace r2d2::location_detector
