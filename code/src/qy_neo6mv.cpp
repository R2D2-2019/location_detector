//
// Created by patrick on 18-3-19.
//

#include "gy_neo6mv.hpp"

gy_neo6mv_c::gy_neo6mv_c(hwlib::target::usart_bus &gps):
    gps(gps),
    nmea_sentence(nmea_sentence_c::OTHER)
{}

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
//            state = input_state::GPRMC;
        }

    }

    while(valid) {
            handle_char(get_char());
        }
    }
    return 1;


}
void gy_neo6mv_c::handle_char(char c) {
    switch (c) {
        case ',': {

            break;
        }
        case '*' : {


            break;
        }
        default: {

            break
        }

    }


}