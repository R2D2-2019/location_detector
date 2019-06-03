#include "uart_nmea_c.hpp"

namespace r2d2::location_detector {

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
                while (usart.char_available()) {
                    usart.receive();
                }
                state = wait_for_start;
                break;

            // wait for a $ sign
            case wait_for_start:
                if (usart.getc() == '$') {
                    gga_sentence[gga_index++] = '$';
                    state = get_message_id;
                }
                break;

            // get the message ID
            case get_message_id:
                if (usart.char_available()) {
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
                        state = clear_buffer;
                    }
                }
                break;

            // check if the message ID is equal to GGA
            case check_message_id:
                if (gga_sentence[2] == 'G' && gga_sentence[3] == 'G' && gga_sentence[4] == 'A') {
                    // if GGA, then send the characters to a string
                    state = get_sentence;
                } else { // if not GGA, clear the string and wait for a $ sign
                    gga_index = 0;
                    state = clear_buffer;
                }
                break;

            case get_sentence:
                if (usart.char_available()) {
                    uart_input = usart.getc();

                    // if a $ is encountered, clear the string and get the
                    // message ID.
                    if (uart_input == '$') {
                        gga_index = 0;
                        gga_sentence[gga_index++] =('$');
                        state = get_message_id;

                        // if the character is a carriage return (ascii code
                        // 13), safe the string and parse it.
                    } else if (uart_input == 13) {
                        last_result = parse_nmea(gga_sentence.begin(), gga_index);
                        return compress(last_result);
                    } else {
                        gga_sentence[gga_index++] =(uart_input);

                        // if the maximum number of characters has been reached,
                        // clear the string and wait for a $ sign
                        if (gga_index >= 100) {
                            gga_index = 0;
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

    uint32_t uart_nmea_c::time_maker(const uint8_t *time, const size_t length) {
        char temp_char;
        uint32_t temp_time = 0;
        for (unsigned int i = 0; i < length; i++) {
            temp_char = time[i];
            temp_char -= '0';
            temp_time += temp_char;
            if (i + 1 < length) {
                temp_time *= 10;
            }
        }
        return temp_time;
    }

    float uart_nmea_c::latitude_longitude_maker(const uint8_t *coordinate, const size_t length) {
        char temp_char;
        uint32_t temp_coord = 0;
        for (unsigned int i = 0; i < length; i++) {
            if (coordinate[i] != '.') {
                temp_char = coordinate[i];
                temp_char -= '0';
                temp_coord += temp_char;
                if (i + 1 < length) {
                    temp_coord *= 10;
                }
            }
        }
        return temp_coord / 1000;
    }

    uint8_t uart_nmea_c::fix_maker(const uint8_t *fix, const size_t length) {
        char temp_char;
        uint8_t fix_int = 0;
        for (unsigned int i = 0; i < length; i++) {
            if (fix[0] == '0') {
                return 0;
            } else if (fix[i] > int('0')) {
                temp_char = fix[i];
                temp_char -= '0';
                fix_int = temp_char;
                return fix_int;
            } else {
                return 0;
            }
        }
        return fix_int;
    }

    uint8_t uart_nmea_c::satellite_maker(const uint8_t *satellites, const size_t length) {
        char temp_char;
        uint8_t number_of_satellites = 0;
        for (unsigned int i = 0; i < length; i++) {
            temp_char = satellites[i];
            temp_char -= '0';
            if (i == 0) {
                number_of_satellites = temp_char;
                if (length > 1) {
                    number_of_satellites *= 10;
                }
            } else if (i == 1) {
                number_of_satellites += temp_char;
            } else {
                return 0;
            }
        }
        return number_of_satellites;
    }

    float
    uart_nmea_c::horizontal_dilution_maker(const uint8_t *dilution, const size_t length) {
        char temp_char;
        float temp_dilution = 0.0;
        for (unsigned int i = 0; i < length; i++) {
            if (dilution[i] != '.') {
                temp_char = dilution[i];
                temp_char -= '0';
                temp_dilution = temp_char;
                if (i + 1 < length) {
                    temp_dilution *= 10;
                }
            }
        }
        return temp_dilution / 10;
    }

    float uart_nmea_c::altitude_geoid_maker(const uint8_t *altitude, const size_t length) {
        char temp_char;
        float temp_altitude = 0.0;
        for (unsigned int i = 0; i < length; i++) {
            if (altitude[0] == '0') {
                return 0.0;
            }
            if (altitude[i] != '.') {
                temp_char = altitude[i];
                temp_char -= '0';
                temp_altitude += temp_char;
                if (i + 1 < length) {
                    temp_altitude *= 10;
                }
            }
        }
        return temp_altitude / 10;
    }

    size_t get_offset_separator(const uint8_t *gps_message, const size_t length, const uint8_t separator){
        for(size_t i = 0; i < length; i++){
            if (gps_message[i] == separator){
                return i + 1;
            }
        }
        return 0;
    }

    gga_s uart_nmea_c::parse_nmea(const uint8_t *gps_message, const size_t length) {
        enum class GGA : uint8_t {
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
        GGA GGAindex = GGA::time;
        for (size_t i = 0; i < length; i++){
            size_t sep_offset = get_offset_separator(gps_message + i, length - i, ',');

            switch (GGAindex){
                case GGA::time: {
                    location.time = time_maker(gps_message + i, sep_offset);
                    break;
                }
                case GGA::latitude: {
                    location.latitude = latitude_longitude_maker(gps_message + i, sep_offset);
                    break;
                }
                case GGA::north_south_hemisphere: {
                    location.north_south_hemisphere = gps_message[i];
                    break;
                }
                case GGA::longitude: {
                    location.longitude = latitude_longitude_maker(gps_message + i, sep_offset);
                    break;
                }
                case GGA::east_west_hemisphere: {
                    location.east_west_hemisphere = gps_message[i];
                    break;
                }
                case GGA::fix_quality: {
                    location.fix_quality = fix_maker(gps_message + i, sep_offset);
                    break;
                }
                case GGA::satellites_tracked: {
                    location.satellites_tracked = satellite_maker(gps_message + i, sep_offset);
                    break;
                }
                case GGA::horizontal_dilution: {
                    location.horizontal_dilution = horizontal_dilution_maker(gps_message + i, sep_offset);
                    break;
                }
                case GGA::altitude: {
                    location.altitude = altitude_geoid_maker(gps_message + i, sep_offset);
                    break;
                }
                case GGA::altitude_measurement: {
                    location.altitude_measurement = gps_message[i];
                    break;
                }
                case GGA::geoid_height: {
                    location.geoid_height = altitude_geoid_maker(gps_message + i, sep_offset);
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
            i += sep_offset;
            GGAindex = static_cast<GGA>(static_cast<uint8_t>(GGAindex) + 1);
        }
        return location;
    }


    frame_coordinate_s uart_nmea_c::compress(float longitude, float latitude,
                                          bool north, bool east,
                                          int16_t altitude) {

        uint8_t longitude_degrees = longitude;
        uint8_t longitude_minutes = (longitude - longitude_degrees) * 60;
        uint8_t longitude_seconds = (
            (longitude - longitude_degrees) * 60 - longitude_minutes
        ) * 60;

        uint8_t longitude_thousandths_second = ((((longitude - longitude_degrees) * 60 - longitude_minutes) * 60) -
             longitude_seconds) *
            1000;

        uint8_t latitude_degrees = latitude;
        uint8_t latitude_minutes = (latitude - latitude_degrees) * 60;
        uint8_t latitude_seconds =
            ((latitude - latitude_degrees) * 60 - latitude_minutes) * 60;
        uint8_t latitude_thousandths_second =
            ((((latitude - latitude_degrees) * 60 - latitude_minutes) * 60) -
             latitude_seconds) *
            1000;

        return frame_coordinate_s{latitude_degrees,
                                  latitude_minutes,
                                  latitude_seconds,
                                  latitude_thousandths_second,
                                  longitude_degrees,
                                  longitude_minutes,
                                  longitude_seconds,
                                  longitude_thousandths_second,
                                  north,
                                  east,
                                  altitude};
    }

} // namespace r2d2::location_detector
