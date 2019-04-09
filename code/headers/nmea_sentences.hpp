//
// Created by patrick on 22-3-19.
//

#pragma once


enum class nmea_sentence_c {
    GPRMC,
    GPGGA, 
    OTHER,
    NONE
};

struct nmea_sentence_info_type_s {
    char name[6];
};

const nmea_sentence_info_type_s & nmea_info(nmea_sentence_c nmea){
    static const nmea_sentence_info_type_s nmea_info_array[ (int)nmea_sentence_c::NONE +1] = {
        {"GPRMC"},
        {"GPGGA"},
        {"OTHER"},
        {"NIHIL"}
    };

    uint_fast8_t n = static_cast<uint_fast8_t>(nmea);
    if (n >= static_cast< uint_fast8_t>(nmea_sentence_c::NONE)) {
        return nmea_info_array[static_cast< uint_fast8_t>(nmea_sentence_c::NONE)];
    }
    return nmea_info_array[n];
}

enum class GPRMC  {



};
