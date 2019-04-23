//
// Created by patrick on 18-3-19.
//

#include "gy_neo6mv.hpp"

gps_location_c::gps_location_c() {}

bool gps_location_c::set_latitude(const char * lat) {
    latitude_degrees_minutes =0;
    char p;
    while(true){
        p = *lat;
        if (p == '.') {
            break;
        } else if (p < '0' || p > '9') {
            return false;
        } else {
            latitude_degrees_minutes*=10;
            latitude_degrees_minutes+=p-'0';
        }
        lat++;
    }
    latitude_seconds = 0;
    lat++;
    while(true){
        p = *lat;
        if (p == '.') {
            break;
        } else if (p < '0' || p > '9') {
            return false;
        } else {
            latitude_seconds*=10;
            latitude_seconds+=p-'0';
        }
        lat++;
    }
    return true;
}

bool gps_location_c::set_hemisphere_NS(char NS) {
    if(NS != 'N' && NS != 'S'){
        return false;
    }
    hemisphere_NS = NS;
    return true;
}

bool gps_location_c::set_longitude(const char *lon) {
    longitude_degrees_minutes =0;
    char p;
    while(true){
        p = *lon;
        if (p == '.') {
            break;
        } else if (p < '0' || p > '9') {
            return false;
        } else {
            longitude_degrees_minutes*=10;
            longitude_degrees_minutes+=p-'0';
        }
        lon++;
    }
    longitude_seconds = 0;
    lon++;
    while(true){
        p = *lon;
        if (p == '.') {
            break;
        } else if (p < '0' || p > '9') {
            return false;
        } else {
            longitude_seconds*=10;
            longitude_seconds+=p-'0';
        }
        lon++;
    }
    return true;
}

bool gps_location_c::set_hemisphere_EW(char EW) {
    if(EW != 'E' && EW != 'S'){
        return false;
    }
    hemisphere_NS = EW;
    return true;
}

bool gps_location_c::set_altitude(const char *alt) {
    altitude = 0;
    char p;
    while(true) {
        p = *alt;
        if (p == '.') {
            continue;
        } else if (p == '\0') {
            altitude*=10;
            return true;
        } else if (p < '0' || p > '9') {
            return false;
        } else {
            altitude*=10;
            altitude+=p-'0';
        }
        alt++;
    }

}


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
    return '\0';
}

int gy_neo6mv_c::parse() {
    bool valid = false;
    while(!valid){
        if(get_char() == '$')  {
            valid = true;
            handle_char('$');
        }

    }

    while(valid_sentence) {
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
            handle_buffer();
            sentence_index++;
            break;
        }
        case '*' : {

            break;
        }
        case '$': {
            nmea_sentence = nmea_sentence_c::NONE;
            sentence_index = 0;
            break;
        }
        default: {
            buffer[buffer_index] = c;
            break;
        }
    }
}

void gy_neo6mv_c::handle_buffer() {
    buffer[buffer_index+1] = '\0';
    switch (nmea_sentence) {
        case nmea_sentence_c::NONE : {
            if (cmp_buffer("GPRMC")) {
                nmea_sentence = nmea_sentence_c::GPRMC;
                sentence_index++;
            } else if(cmp_buffer("GPGGA")){
                nmea_sentence = nmea_sentence_c::GPGGA;
                sentence_index++;
            } else {
                valid_sentence = false;
            }
            break;
        }
        case nmea_sentence_c::GPRMC : {
            switch (sentence_index){
                case (2):{
                    if (!cmp_buffer("A")){
                        valid_sentence = false;
                    }
                    break;
                } case (3):{
                    location.set_latitude(buffer);
                    break;
                } case (4): {
                    location.set_hemisphere_NS(*buffer);
                    break;
                } case (5): {
                    location.set_longitude(buffer);
                    break;
                } case (6): {
                    location.set_hemisphere_EW(*buffer);
                    break;
                }
            }
            break;
        }
        case nmea_sentence_c::GPGGA : {
            switch (sentence_index){
                case (2):{
                    location.set_latitude(buffer);
                    break;
                }
                case (3):{
                    location.set_hemisphere_NS(*buffer);
                    break;
                }
                case (4):{
                    location.set_altitude(buffer);
                    break;
                }
                case(5):{
                    location.set_hemisphere_EW(*buffer);
                    break;
                }
                case (9):{
                    location.set_altitude(buffer);
                    break;
                }
            }
            break;
        }

        default: {

            break;
        }
    }

}


bool gy_neo6mv_c::cmp_buffer(const char* rhs) {
    for (unsigned int i =0; i < sizeof(rhs); i++) {
        if (rhs[i] != buffer[i]){
            return false;
        }
    }
    return true;

}