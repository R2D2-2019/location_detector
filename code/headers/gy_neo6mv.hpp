#pragma once

#include <hwlib.hpp>
#include <nmea_sentences.hpp>
#include <hardware_usart.hpp>

class gps_location_c {
private:
    int latitude_degrees_minutes = 0 ;
    int latitude_seconds = 0;
    char hemisphere_NS = 'N';
    int longitude_degrees_minutes = 0;
    int longitude_seconds = 0;
    char hemisperhe_EW = 'E';
    int altitude = 0;
public:
    gps_location_c();
    bool set_latitude(const char * lat);
    bool set_hemisphere_NS(char NS);
    bool set_longitude(const char * lon);
    bool set_hemisphere_EW(char EW);
    bool set_altitude(const char * alt);

};

class gy_neo6mv_c {
private:
    r2d2::hardware_usart_c &gps;
    nmea_sentence_c nmea_sentence;
    bool valid_sentence;
    char buffer[16];
    int sentence_index;
    gps_location_c location;

    char get_char();
    int parse();
    void handle_char(char c);
    int buffer_index =0;
    void handle_buffer();
    bool cmp_buffer(const char* rhs);
public:
    gy_neo6mv_c(r2d2::hardware_usart_c &gps);

};
