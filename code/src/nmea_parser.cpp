#include <nmea_parser.hpp>
#include <string.hpp>

using r2d2::string::atof;
using r2d2::string::atoi;
using r2d2::string::get_offset_separator;

namespace r2d2::location {
    gga_s nmea_parser_c::parse_nmea(const uint8_t *gps_message, const size_t length) {
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
}