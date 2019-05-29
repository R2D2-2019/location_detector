#include "location_detector.hpp"

namespace r2d2::location_detector {

    frame_coordinate_s module_c::compress(float longitude, float latitude,
                                          bool north, bool east,
                                          int16_t altitude) {
        frame_coordinate_s compressed_frame;

        compressed_frame.altitude = altitude;
        compressed_frame.north_south_hemisphere = north;
        compressed_frame.east_west_hemisphere = east;

        compressed_frame.
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