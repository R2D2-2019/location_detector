#pragma once

#include <usart_connection.hpp>

namespace r2d2::location {
    class nmea_listener {
    protected:
        constexpr static uint8_t start_byte = '$';
        constexpr static uint8_t end_byte = '\n';
        constexpr static size_t buffer_size = 100;

        usart::usart_connection_c &usart;

        /**
         * @brief buffer for storing the nmea string
         *
         */
        uint8_t buffer[buffer_size];

        /**
         * @brief
         *
         */
        size_t index = 0;

        /**
         * @brief bool for storing if a string is avaialable
         *
         */
        bool available;

        /**
         * @brief bool for bookkeeping if we are in the middle of a string
         *
         */
        bool has_startbyte = false;

    public:
        nmea_listener(usart::usart_connection_c &usart) : usart(usart) {
        }

        bool nmea_available() {
            return available;
        }

        /**
         * @brief Get a pointer to a nmea string
         *
         * @return const uint8_t*
         */
        const uint8_t *get_nmea_string() {
            if (!nmea_available()) {
                return nullptr;
            }

            return buffer;
        }

        /**
         * @brief mark the current string as read. To get new data from the
         * usart.
         *
         */
        void read_nmea_string() {
            available = false;
            index = 0;
        }

        /**
         * @brief clears the usart from the old data. As when a overflow occurs
         * in the ringbuffer in the hardware usart we should remove all data to
         * readout the new data.
         *
         */
        void clear_all_usart() {
            while (usart.available()) {
                // read data and forget it
                (void)usart.receive();
            }
        }

        /**
         * @brief returns the length of the current nmea string
         * 
         */
        size_t length() {
            return index;
        }

        /**
         * @brief update the internal buffer with the data from the usart
         * buffer. This won't update the buffer if a string is already present
         * in the local buffer.
         *
         */
        void update_buffer() {
            // don't check usart if a string is already available
            if (nmea_available()) {
                return;
            }

            while (usart.available()) {
                // receive data from the usart
                uint8_t data = usart.receive();

                if (data == end_byte) {
                    // we now have a nmea string
                    available = true;
                    has_startbyte = false;
                    return;
                }

                if (data == start_byte) {
                    has_startbyte = true;
                }

                if (!has_startbyte) {
                    continue;
                }

                buffer[index++] = data;

                // check if something went wrong as a nmea string shouldn't be
                // buffer_size long
                if (index >= buffer_size) {
                    has_startbyte = false;
                    return;
                }
            }
        }
    };
} // namespace r2d2::location