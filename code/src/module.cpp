#include "module.hpp"

namespace r2d2::location_detector {
    void module_c::process() {
        // check for frame
        while (comm.has_data()) {
            auto frame = comm.get_data();

            if (true) { // TODO: if location request, call get_location
                frame_coordinate_s location_frame = nmea.get_location();

                // send the location frame on the CAN-bus
                comm.send(location_frame);
            }
        }
    }

} // namespace r2d2::location_detector