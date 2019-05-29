#include "location_detector.hpp"

namespace r2d2::location_detector {

    frame_coordinate_s module_c::compress(float longitude, float latitude, bool north, bool east, int16_t altitude) {

        uint8_t longitude_degrees = longitude;
        uint8_t longitude_minutes = (longitude - longitude_degrees) * 60;
        uint8_t longitude_seconds = ((longitude - longitude_degrees) * 60 - longitude_minutes) * 60;
        uint8_t longitude_thousandths_second = (((( longitude - longitude_degrees) * 60 - longitude_minutes) * 60) - longitude_seconds) * 1000;

        uint8_t latitude_degrees = latitude;
        uint8_t latitude_minutes = (latitude - latitude_degrees) * 60;
        uint8_t latitude_seconds = ((latitude - latitude_degrees) * 60 - latitude_minutes) * 60;
        uint8_t latitude_thousandths_second = (((( latitude - latitude_degrees) * 60 - latitude_minutes) * 60) - latitude_seconds) * 1000;

        return frame_coordinate_s {latitude_degrees, latitude_minutes, latitude_seconds, latitude_thousandths_second, longitude_degrees, longitude_minutes, longitude_seconds,
                                    longitude_thousandths_second, north, east, altitude};

    }

    void module_c::process() {
        // check for frame
        while (comm.has_data()) {
            auto frame = comm.get_data();

            if (true) { // TODO: if location request, call get_location
                frame_coordinate_s location_frame = get_location();

                // send the location frame on the CAN-bus
                comm.send(location_frame);
            }
        }
    }

} // namespace r2d2::location_detector