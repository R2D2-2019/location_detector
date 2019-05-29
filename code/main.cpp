#include "hwlib.hpp"

uint32_t time_maker(const hwlib::string<10> &time){
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
float latitude_longitude_maker(const hwlib::string<15> &coordinate){
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

uint8_t fix_maker(const hwlib::string<10> &fix){
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

uint8_t satellite_maker(const hwlib::string<10> &satellites){
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

float horizontal_dilution_maker(const hwlib::string<10> &dilution){
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

float altitude_geoid_maker(const hwlib::string<10> &altitude){
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
gga_s parse_nmea(const hwlib::string<65> &gps_message){
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
  /////////////////////////////////////////////////////////////
  /// some couts for debugging and testing, will be removed ///
  /////////////////////////////////////////////////////////////
  hwlib::cout << "Time is: " << '\t'<< '\t' << '\t' << time << hwlib::endl;
  hwlib::cout << "Latitude is: " << '\t'<< '\t'<< '\t' << latitude << hwlib::endl;
  hwlib::cout << "Hemisphere is: "  << '\t'<< '\t'<< '\t'<< north_south_hemisphere << hwlib::endl;
  hwlib::cout << "Longitude is: "<< '\t'<< '\t'<< '\t' << longitude << hwlib::endl;
  hwlib::cout << "Hemisphere is: "<< '\t'<< '\t'<< '\t' << east_west_hemisphere << hwlib::endl;
  hwlib::cout << "Fix is: " << '\t'<< '\t'<< '\t'<< fix_quality << hwlib::endl;
  hwlib::cout << "Number of satellites: " << '\t'<< '\t'<<  satellites_tracked << hwlib::endl;
  hwlib::cout << "Horizontal dilution: " << '\t'<< '\t'<<  horizontal_dilution <<hwlib::endl;
  hwlib::cout << "Altitude is: "<< '\t'<< '\t' << '\t'<< altitude << hwlib::endl; 
  hwlib::cout << "Altitude_measurement is: " << '\t'<< altitude_measurement << hwlib::endl;
  hwlib::cout << "Geoid is: " << '\t'<< '\t'<< '\t'<< geoid_height <<hwlib::endl;
  hwlib::cout << "Geoid is: " << '\t'<< '\t'<< '\t'<< geoid_height_measurement <<hwlib::endl;
  hwlib::cout << "amount of commas "<< '\t'<< '\t' << comma_counter << hwlib::endl;

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

int main(void) {
  // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;
    hwlib::wait_ms(1000);
    const hwlib::string <100> gps_message = "$GPGGA,123519,4807.038,N,01131.000,E,5,14,0.9,545.4,M,46.9,M,,*47"; //GSS Formatted message from module to test function

    for (unsigned int i = 0; i < 1; i++){
        hwlib::wait_ms(1000);
        hwlib::cout << "this works via arduino" << hwlib::endl;
        hwlib::wait_ms(1000);
      hwlib::cout << gps_message.length() << hwlib::endl;
    }
    gga_s checkit = parse_nmea(gps_message);
    hwlib::cout << "test cout \n";
    hwlib::cout << checkit.time << hwlib::endl;
}