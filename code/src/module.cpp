#include "module.hpp"

namespace r2d2::location {
    void module_c::process() {
        // check for frame
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

        // process any available data. This is done so we don't send any old
        // location if we didn't get a request for a long time.
        auto gga = nmea.get_location();

        // send data if we have a request
        if (request_triggered) {
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