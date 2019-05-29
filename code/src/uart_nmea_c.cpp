#include "uart_nmea_c.hpp"

namespace r2d2::location_detector {

    uart_nmea_c::uart_nmea_c(usart::usart_connection_c &GNSS_uart_port)
        : gga_sentence(), GNSS_uart_port(GNSS_uart_port) {
    }

    frame_coordinate_s uart_nmea_c::compress(const gga_s &source) {
        return compress(source.longitude, source.latitude,
                        (source.north_south_hemisphere == "N"),
                        (source.east_west_hemisphere == "E"), source.altitude);
    }

    frame_coordinate_s uart_nmea_c::get_location() {
        enum get_location_state {
            clear_buffer,
            wait_for_start,
            get_message_id,
            check_message_id,
            get_sentence
        };
        get_location_state state = clear_buffer;
        char uart_input = 0;

        while (true) {
            switch (state) {
            // clear the uart buffer
            case clear_buffer:
                while (GNSS_uart_port.char_available()) {
                    GNSS_uart_port.receive()
                }
                state = wait_for_start;
                break;

            // wait for a $ sign
            case wait_for_start:
                if (GNSS_uart_port.getc() == '$') {
                    gga_sentence.append('$');
                    state = get_message_id;
                }
                break;

            // get the message ID
            case get_message_id:
                if (GNSS_uart_port.char_available()) {
                    gga_sentence.append(GNSS_uart_port.getc());
                    if (gga_sentence[gga_sentence.length() - 1] == ',') {
                        // a comma indicates the end of the message id
                        state = check_message_id;

                        // the message ID, together with the '$' at the start
                        // and ',' at the end, has a length of 7 characters. If
                        // the ID is longer, it cannot be the ID we are looking
                        // for.
                    } else if (gga_sentence.length() > 7) {
                        gga_sentence.clear();
                        state = clear_buffer;
                    }
                }
                break;

            // check if the message ID is equal to GGA
            case check_message_id:
                if (gga_sentence.find_at("GGA", 2)) {
                    // if GGA, then send the characters to a string
                    state = get_sentence;
                } else { // if not GGA, clear the string and wait for a $ sign
                    gga_sentence.clear();
                    state = clear_buffer;
                }
                break;

            case get_sentence:
                if (GNSS_uart_port.char_available()) {
                    uart_input = GNSS_uart_port.getc();

                    // if a $ is encountered, clear the string and get the
                    // message ID.
                    if (uart_input == '$') {
                        gga_sentence.clear();
                        gga_sentence.append('$');
                        state = get_message_id;

                        // if the character is a carriage return (ascii code
                        // 13), safe the string and parse it.
                    } else if (uart_input == 13) {
                        last_result = parse_nmea(gga_sentence);
                        return compress(last_result);
                    } else {
                        gga_sentence.append(uart_input);

                        // if the maximum number of characters has been reached,
                        // clear the string and wait for a $ sign
                        if (gga_sentence.length() >= 100) {
                            gga_sentence.clear();
                            state = wait_for_start;
                        }
                    }
                }
                break;

            default:
                break;
            }
        }

    }
    
    uint32_t uart_nmea_c::time_maker(const hwlib::string<10> &time){
    char temp_char;
    uint32_t temp_time = 0;
    for(unsigned int i = 0; i < time.length(); i++){ 
        temp_char = time[i];
        temp_char -= '0';
        temp_time += temp_char;
        if(i+1 < time.length()){
        temp_time *= 10;
        }
    }
    return temp_time;
    }
    float uart_nmea_c::latitude_longitude_maker(const hwlib::string<15> &coordinate){
    char temp_char;
    uint32_t temp_coord = 0;
    for(unsigned int i = 0; i < coordinate.length(); i++){
        if(coordinate[i] != '.'){
        temp_char = coordinate[i];
        temp_char -= '0';
        temp_coord += temp_char;
        if(i+1 < coordinate.length()){
        temp_coord *= 10;
        }
        }
    }
    return temp_coord / 1000;
    }

    uint8_t uart_nmea_c::fix_maker(const hwlib::string<10> &fix){
    char temp_char;
    uint8_t fix_int = 0;
    for(unsigned int i = 0; i < fix.length(); i++){
        if(fix[0] == '0'){
        return 0;
        }
        else if(fix[i] > int('0')){
        temp_char = fix[i];
        temp_char -= '0';
        fix_int = temp_char;
        return fix_int;
        }
        else{
        return 0;
        }
    }
    return fix_int;
    }

    uint8_t uart_nmea_c::satellite_maker(const hwlib::string<10> &satellites){
    char temp_char;
    uint8_t number_of_satellites = 0;
    for(unsigned int i = 0; i < satellites.length(); i++){
        temp_char = satellites[i];
        temp_char -= '0';
        if(i == 0){
        number_of_satellites = temp_char;
        if(satellites.length() > 1){
            number_of_satellites *= 10;
        }
        }
        else if(i == 1){
        number_of_satellites += temp_char;
        }
        else{
        return 0;
        }
    }
    return number_of_satellites;
    }

    float uart_nmea_c::horizontal_dilution_maker(const hwlib::string<10> &dilution){
    char temp_char;
    float temp_dilution = 0.0;
    for(unsigned int i = 0; i < dilution.length(); i++){
        if(dilution != '.') {
        temp_char = dilution[i];
        temp_char -= '0';
        temp_dilution = temp_char;
        if(i+1 < dilution.length()) {
            temp_dilution *= 10;
        }
        }
    }
    return temp_dilution / 10;
    }

    float uart_nmea_c::altitude_geoid_maker(const hwlib::string<10> &altitude){
    char temp_char;
    float temp_altitude = 0.0;
    for(unsigned int i = 0; i < altitude.length(); i++){
        if(altitude[0] == '0'){
        return 0.0;
        }
        if(altitude[i] != '.') {
        temp_char = altitude[i];
        temp_char -= '0';
        temp_altitude += temp_char;
        if(i+1 < altitude.length()) {
            temp_altitude *= 10;
        }
        }
    }
    return temp_altitude / 10 ;
    }
    gga_s uart_nmea_c::parse_nmea(const hwlib::string<0> &gps_message){
    uint8_t           comma_counter = 0;
    hwlib::string<10> time ;
    hwlib::string<10> latitude;
    char              north_south_hemisphere;
    hwlib::string<10> longitude;
    char              east_west_hemisphere;
    hwlib::string<10> fix_quality;
    hwlib::string<10> satellites_tracked;
    hwlib::string<10> horizontal_dilution;
    hwlib::string<10> altitude;
    char              altitude_measurement;
    hwlib::string<10> geoid_height;
    char              geoid_height_measurement;
    for(unsigned int i = 0; i < gps_message.length(); i++){
        if(gps_message[i] == ','){
        comma_counter++;
        i++;
        }
        switch (comma_counter){
        case 1: {
            time.append(gps_message[i]);
        }break;
        case 2: {
            latitude.append(gps_message[i]);
        }break;
        case 3: {
            north_south_hemisphere = gps_message[i];
        }break;
        case 4: {
            longitude.append(gps_message[i]);
        }break;
        case 5: {
            east_west_hemisphere = gps_message[i];
        }break;
        case 6: {
            fix_quality.append(gps_message[i]);
        }break;
        case 7: {
            satellites_tracked.append(gps_message[i]);
        }break;
        case 8: {
            horizontal_dilution.append(gps_message[i]);
        }break;
        case 9:  {
            altitude.append(gps_message[i]);
        }break;
        case 10:{
            altitude_measurement = gps_message[i];
        }break;
        case 11:{
            geoid_height.append(gps_message[i]);
        }
        case 12: {
            geoid_height_measurement = gps_message[i];
        }break;
        }
    }
    
    gga_s location_package;
    location_package.time = time_maker(time);
    location_package.latitude = latitude_longitude_maker(latitude);
    location_package.north_south_hemisphere = north_south_hemisphere;
    location_package.longitude = latitude_longitude_maker(longitude);
    location_package.east_west_hemisphere = east_west_hemisphere;
    location_package.fix_quality = fix_maker(fix_quality);
    location_package.satellites_tracked = satellite_maker(satellites_tracked);
    location_package.horizontal_dilution = horizontal_dilution_maker(horizontal_dilution);
    location_package.altitude = altitude_geoid_maker(altitude);
    location_package.altitude_measurement = altitude_measurement;
    location_package.geoid_height = altitude_geoid_maker(geoid_height);
    location_package.geoid_height_measurement = geoid_height_measurement;

    return location_package;
    }

} // namespace r2d2::location_detector
