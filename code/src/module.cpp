#include "module.hpp"
#include <iostream>

namespace r2d2::location {
    void module_c::process() {
        // check for frame
        std::cout << "comm data: " << comm.has_data() << "\n";
        while (comm.has_data()) {
            auto frame = comm.get_data();
            if (request_triggered) {
                // no need to check anything anymore as we need to send a
                // request anyway.
                continue;
            }

            if (frame.request) {
                request_triggered = true;
            }
        }
        if (request_triggered) {
            auto gga = nmea.get_location();

            // check if we have a valid location
            if (gga.fix_quality == 0) {
                // gps signal is invalid. So we don't have data yet or we dont
                // have a fix.
                return;
            }

            // convert gga data to a frame
            auto frame = nmea.gga_to_frame(gga);

            // send the frame on the bus
            comm.send(frame);

            request_triggered = false;
        }
    }

} // namespace r2d2::location