#pragma once

#include <hwlib.hpp>
#include <nmea_sentences.hpp>
#include <hardware_usart.hpp>

struct gps_location_s {
    char latitude_degrees_minutes[5] ;
    char latitude_seconds[5];
    char longitude_degrees_minutes[5];
    char longitude_seconds[5];
};

class gy_neo6mv_c {
private:
    r2d2::hardware_usart_c &gps;
    nmea_sentence_c nmea_sentence;
    bool valid_sentence;
    char buffer[16];
    int sentence_index;

    char get_char();
    int parse();
    void handle_char(char c);
    int buffer_index =0;
    void handle_buffer();
    bool cmp_buffer( char* rhs);
public:
    gy_neo6mv_c(r2d2::hardware_usart_c &gps);

};
