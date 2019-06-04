#pragma once

#include <cstdint>

namespace r2d2::string {
    /**
     * @brief Get the offset of the separator
     * returns 0 if no seperator is found
     *
     * @param string
     * @param length
     * @param separator
     * @return size_t returns the length when the seperator is found in the
     * array
     */
    size_t get_offset_separator(const uint8_t *string, const size_t length,
                                const uint8_t separator);

    /**
     * @brief Get a number from a string any non number character will be
     * ignored
     *
     * @param string
     * @param length
     * @return int32_t
     */
    int32_t atoi(const uint8_t *string, const size_t length);

    /**
     * @brief Get a float from a string any non number character will be
     * ignored and will be splitted on the '.' in the string.
     *
     * @param string
     * @param length
     * @return int32_t
     */
    float atof(const uint8_t *string, const size_t length);
}