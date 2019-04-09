//
// Created by patrick on 18-3-19.
//

#include "gy_neo6mv.hpp"

gy_neo6mv_c::gy_neo6mv_c(r2d2::hardware_usart_c &gps):
    gps(gps),
    nmea_sentence(nmea_sentence_c::OTHER)
{
    valid_sentence = true;
}

char gy_neo6mv_c::get_char() {
    if (gps.char_available()) {
        return gps.getc();
    }
}

int gy_neo6mv_c::parse() {
    bool valid = false;
    char tmp_char;
    while(!valid){
        if(get_char() == '$')  {
            valid = true;
            handle_char('$');
        }

    }

    while(valid) {
        handle_char(get_char());
    }

    return 1;


}
void gy_neo6mv_c::handle_char(char c) {
    if (!valid_sentence) {
        return;

    }
    switch (c) {
        case ',': {

            break;
        }
        case '*' : {

            break;
        }
        case '$': {
            nmea_sentence = nmea_sentence_c::NONE;

            break;
        }
        default: {
            buffer[buffer_index] = c;
            break;
        }
    }
}

void gy_neo6mv_c::handle_buffer() {
    switch (nmea_sentence) {
        case nmea_sentence_c::NONE : {
            if (cmp_buffer("GPRMC")) {
                nmea_sentence = nmea_sentence_c::GPRMC;
                sentence_index = 1;
            } else if(cmp_buffer("GPGGA")){
                nmea_sentence = nmea_sentence_c::GPGGA;
                sentence_index = 1;
            } else {
                valid_sentence = false;
            }
            break;
        }
        case nmea_sentence_c::GPRMC : {

            break;
        }
        case nmea_sentence_c::GPGGA : {

            break;
        }
    }

}


bool gy_neo6mv_c::cmp_buffer(const char* rhs) {
    for (int i =0; i < sizeof(rhs); i++) {
        if (rhs[i] != buffer[i]){
            return false;
        }
    }
    return true;

}