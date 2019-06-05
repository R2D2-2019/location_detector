#include <cmath>
#include <string.hpp>

namespace r2d2::location {
    size_t get_offset_separator(const uint8_t *gps_message, 
                                const size_t length,
                                const uint8_t separator) {

        for (size_t i = 0; i < length; i++) {
            if (gps_message[i] == separator) {
                return i + 1;
            }
        }
        return 0;
    }

    int32_t atoi(const uint8_t *string, const size_t length) {
        int32_t res = 0;

        if (!length) {
            return res;
        }

        for (size_t i = 0; i < length; i++) {
            if (string[i] >= '0' && string[i] <= '9') {
                res = res * 10 + string[i] - '0';
            }
        }

        return res * (string[0] == '-' ? -1 : 1);
    }

    float atof(const uint8_t *string, const size_t length) {
        size_t offset = get_offset_separator(string, length, '.');

        return atoi(string, length) /
               (offset ? pow(float(10), int(length - offset) - 1) : 1);
    }
} // namespace r2d2::location