#include <nmea_parser.hpp>
#include <string.hpp>
#include <cmath>

namespace r2d2::location {
    decimal_degrees get_degrees(const uint8_t *data, size_t length) {
        size_t comma_offset = get_offset_separator(data, length, '.');
        decimal_degrees ret{};
        uint32_t degrees_minutes;

        if (comma_offset) {
            // get the data before the comma
            degrees_minutes = atoi(data, comma_offset);

            // get the data after the comma
            ret.tenthousandths = atoi(data + comma_offset, length - comma_offset);

            // check if the length of the string is in the tenthousandths
            uint16_t after_comma = length - comma_offset;

            if (after_comma < 5) {
                // multiply so the value is in the correct place
                ret.tenthousandths *= pow(10, 5 - after_comma);
            }
            
        } else {
            degrees_minutes = atoi(data, length);
        }

        ret.degrees = degrees_minutes / 100;
        ret.minutes = degrees_minutes - (ret.degrees * 100);

        return ret; 
    }

    gga_s nmea_parser_c::parse_nmea(const uint8_t *gps_message,
                                    const size_t length) {
        gga_s location{};
        GGA GGAindex = GGA::sentence;

        for (size_t i = 0; i < length; i++) {
            size_t sep_offset = get_offset_separator(gps_message + i, length - i, ',');

            switch (GGAindex) {
            case GGA::sentence: {
                // Current sentence, currently only supports GGA

                // check if the message ID is equal to GGA
                if (gps_message[3] == 'G' && gps_message[4] == 'G' &&
                    gps_message[5] == 'A') {
                    // if GGA, go to next step
                    break;
                }

                // if not GGA, return
                return gga_s{};
            }
            case GGA::time: {
                location.time = atoi(gps_message + i, sep_offset);
                break;
            }
            case GGA::latitude: {
                location.latitude = get_degrees(gps_message + i, sep_offset);
                break;
            }
            case GGA::north_south_hemisphere: {
                location.is_north_hemisphere = gps_message[i] == 'N';
                break;
            }
            case GGA::longitude: {
                location.longitude = get_degrees(gps_message + i, sep_offset);
                break;
            }
            case GGA::east_west_hemisphere: {
                location.is_east_hemisphere = gps_message[i] == 'E';
                break;
            }
            case GGA::fix_quality: {
                location.fix = static_cast<fix_status>(atoi(gps_message + i, sep_offset));
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
                location.altitude_unit = gps_message[i];
                break;
            }
            case GGA::geoid_height: {
                location.geoid_height = atof(gps_message + i, sep_offset);
                break;
            }
            case GGA::geoid_height_measurement: {
                location.geoid_height_unit = gps_message[i];
                break;
            }
            case GGA::diff_age: {
                // this parameter is not transmitted if we dont have a dgps fix
                // or higher (dgps == 2 so higher than or 2).
                if (static_cast<uint8_t>(location.fix) <= 1) {
                    break;
                }

                location.diff_age = atoi(gps_message + i, sep_offset);
                break;
            }
            case GGA::diff_id: {
                // this parameter is not transmitted if we dont have a dgps fix
                // or higher (dgps == 2 so higher than or 2).
                if (static_cast<uint8_t>(location.fix) <= 1) {
                    break;
                }

                location.diff_id = atoi(gps_message + i, sep_offset);
                break;
            }
            default: { 
                return location; 
            }
            }

            i += (sep_offset ? sep_offset - 1 : 0);
            GGAindex = static_cast<GGA>(static_cast<uint8_t>(GGAindex) + 1);
        }
        return location;
    }
} // namespace r2d2::location