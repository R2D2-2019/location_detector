#include "hwlib.hpp"
#include <hardware_usart.hpp>

struct location_s{
  hwlib::string<10> protocol_type;        // 0 type of protocol received
  uint32_t          time;                 // 1 current time when location was sent
  uint32_t           latitude;             // 2 east to west
  char              north_or_south;       // 3 'N' will be north and 'S' will be south
  uint32_t           longitude;            // 4 north to south
  char              east_or_west;         // 5 'E' will be east and 'E' will be west
  uint8_t           fix;                  // 6 Satellite 'fix' w/e that is...
  uint8_t           number_of_satellites; // 7 Number of satellites tracked
  uint8_t          horizontal_dilution;  // 8 Not even a clue what this means at the moment
  uint16_t          altitude;             // 9 height relative to oceanic water surface
  uint32_t          geoid;                // 10 height of geoid
};
// hwlib::string<10> protocol_namer(const hwlib::string & protocol)const{
//   hwlib::cout<< protocol << hwlib::endl;
// }
////////////////////////////////////////////////////
//// Function to make the time that is a string ////
//// an integer for the location struct ////////////
////////////////////////////////////////////////////
uint32_t time_maker(const hwlib::string<6> & time){
  char temp_char;                    //Holds a temporary char from the time string to calculate with
  uint32_t temp_time = 0;           //Holds a temporary value to place into the new_time
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
////////////////////////////////////////////////////////////////////////////////
//////// function to make a latitude or longitude string an integer ////////////
/////////////////// to place into the frame struct /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
uint32_t latitude_longitude_maker(const hwlib::string<15> & coordinate){
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
  return temp_coord;
}
///////////////////////////////////////////////////////////////
//// function to parse a GPS string to correct sub strings ////
///////////////////////////////////////////////////////////////
void parser(const hwlib::string<65> & gps_message){
  uint8_t              comma_counter = 0;
  hwlib::string<10>    protocol_temp;
  hwlib::string<10>    time_temp;
  hwlib::string<10>    latitude_temp;
  hwlib::string<10>    north_or_south_temp;
  hwlib::string<10>    longitude_temp;
  hwlib::string<10>    east_or_west_temp;
  hwlib::string<10>    fix_temp;
  hwlib::string<10>    number_of_satellites_temp;
  hwlib::string<10>    horizontal_dilution_temp;
  hwlib::string<10>    altitude_temp;
  hwlib::string<10>    geoid_temp;


  for(unsigned int i = 0; i < gps_message.length(); i++){
    if(gps_message[i] == ','){
      comma_counter++;
      i++;
    }
    //////////////////////////////////////////////////////////
    /// cutting up the gps message, due to NMAE GGS format ///
    /////// I will use the provided comma delimiter //////////
    ////////////////////////////////////////////////////////// 
    switch (comma_counter){
      case 0:{
        protocol_temp.append(gps_message[i]);
      }break;
      case 1:{
        time_temp.append(gps_message[i]);
      }break;
      case 2:{
        latitude_temp.append(gps_message[i]);
      }break;
      case 3:{
        north_or_south_temp = gps_message[i];
      }break;
      case 4:{
        longitude_temp.append(gps_message[i]);
      }break;
      case 5:{
        east_or_west_temp = gps_message[i];
      }break;
      case 6:{
        fix_temp.append(gps_message[i]);
      }break;
      case 7:{
        number_of_satellites_temp.append(gps_message[i]);
      }break;
      case 8:{
        horizontal_dilution_temp.append(gps_message[i]);
      }
      case 9:{
        altitude_temp.append(gps_message[i]);
      }break;
      case 11:{
        geoid_temp.append(gps_message[i]);
      }
    }
  }

  /////////////////////////////////////////////////////////////
  /// some couts for debugging and testing, will be removed ///
  /////////////////////////////////////////////////////////////
  hwlib::cout << "Protocol used: " << protocol_temp << hwlib::endl;
  hwlib::cout << "Time is: " << time_temp << hwlib::endl;
  hwlib::cout << "Latitude is: " << latitude_temp << hwlib::endl;
  hwlib::cout << "Hemisphere is: " << north_or_south_temp << hwlib::endl;
  hwlib::cout << "Longitude is: " << longitude_temp << hwlib::endl;
  hwlib::cout << "Hemisphere is: " << east_or_west_temp << hwlib::endl;
  hwlib::cout << "Fix is: " << fix_temp << hwlib::endl;
  hwlib::cout << "Number of satellites: "  << number_of_satellites_temp << hwlib::endl;
  hwlib::cout << "Horizontal dilution: " << horizontal_dilution_temp <<hwlib::endl;
  hwlib::cout << "Altitude is: " << altitude_temp << hwlib::endl;
  hwlib::cout << "Geoid is: " << geoid_temp <<hwlib::endl;
  hwlib::cout << "amount of commas " << comma_counter << hwlib::endl;
  hwlib::cout << "==============================================" << hwlib::endl;
  hwlib::cout << latitude_longitude_maker(latitude_temp) << hwlib::endl;
  hwlib::cout << latitude_longitude_maker(longitude_temp) << hwlib::endl;
  hwlib::cout << time_maker(time_temp) << hwlib::endl;
}

int main(void) {
  // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;
    hwlib::wait_ms(1000);
    hwlib::string <255> gps_message = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47"; //GSS Formatted message from module to test function
    for (unsigned int i = 0; i < 1; i++){
        hwlib::wait_ms(1000);
        hwlib::cout << "this works via arduino" << hwlib::endl;
        hwlib::wait_ms(1000);
      hwlib::cout << gps_message.length() << hwlib::endl;
    }
    parser(gps_message);
}