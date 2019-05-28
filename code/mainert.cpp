#include "hwlib.hpp"
#include <hardware_usart.hpp>

struct gga_s{
  uint32_t  time;                 // 1 current time when location was sent
  float     latitude;             // 2 east to west
  char      north_or_south;       // 3 'N' will be north and 'S' will be south
  float     longitude;            // 4 north to south
  char      east_or_west;         // 5 'E' will be east and 'E' will be west
  uint8_t   fix;                  // 6 Satellite 'fix' w/e that is...
  uint8_t   number_of_satellites; // 7 Number of satellites tracked
  float     horizontal_dilution;  // 8 Not even a clue what this means at the moment
  float     altitude;             // 9 height relative to oceanic water surface
  char      altitude_measurement;
  float     geoid;                // 10 height of geoid
  char      geoid_height_measurement;
};

void reparse(char * array) {
    int          comma_counter = 0;
    int          i = 0;
    int          d = 0;
    gga_s        location_package;
    (void)location_package;
    char * time = nullptr;
    char * latitude= nullptr;
    char * north_south_hemisphere= nullptr;
    char * longitude= nullptr;
    char * east_west_hemisphere= nullptr;
    char * fix_quality= nullptr;
    char * satellites_tracked= nullptr;
    char * horizontal_dilution= nullptr;
    char * altitude= nullptr;
    char * altitude_measurement= nullptr;
    char * geoid_height= nullptr;
    char * geoid_height_measurement= nullptr;

    while(array[i] != '\0') {
        
        if(array[i] == ','){
            comma_counter++;
            i++;
            d = 0;
        }
        hwlib::cout << array[i];
        switch (comma_counter) {
            case 1: {
                time = array + i;
            }break;
            case 2: {
                latitude[d] = array[i];
            }break;
            case 3: {
                north_south_hemisphere[d] = array[i];
            }break;
            case 4: {
                longitude[d] = array[i];
            }break;
            case 5: {
                east_west_hemisphere[d] = array[i];
            }break;
            case 6: {
                fix_quality[d] = array[i];
            }break;
            case 7: {
                satellites_tracked[d] = array[i];
            }break;
            case 8: {
                horizontal_dilution[d] = array[i];
            }break;
            case 9: {
                altitude[d] = array[i];
            }break;
            case 10: {
                altitude_measurement[d] = array[i];
            }break;
            case 11: {
                geoid_height[d] = array[i];
            }break;
            case 12: {
                geoid_height_measurement[d] = array[i];
            }break;
        }
        d++;
        i++;
    }
    hwlib::cout << hwlib::endl;
    hwlib::cout << 'a' << time << ' ' << longitude << ' ' << north_south_hemisphere << ' ' << latitude << ' ' << east_west_hemisphere << hwlib::endl;
    hwlib::cout << 'b' <<fix_quality << ' ' << satellites_tracked << ' ' << horizontal_dilution << ' ' << altitude << ' ' << altitude_measurement << ' ' << hwlib::endl;
    hwlib::cout << 'c' <<geoid_height << ' '  << geoid_height_measurement << hwlib::endl;
    for(unsigned int j = 0; j < 5; j++){
        hwlib::cout<< time[j] << j;
    }
    hwlib::cout << hwlib::endl;
}




//int main(void) {
  // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;
    hwlib::wait_ms(1000);
    const char *arraytje = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    reparse(arraytje);

    hwlib::cout << "shitzzzz" << hwlib::endl;
    const hwlib::string<7> test = "onetwo\0";
    const char * test_pointer = test.begin();
    hwlib::cout << test_pointer[0] << hwlib::endl;
    hwlib::cout << test_pointer[1] << hwlib::endl;
    hwlib::cout << test_pointer[2] << hwlib::endl;
    hwlib::cout << test_pointer[3] << hwlib::endl;
    hwlib::cout << test_pointer[4] << hwlib::endl;
    hwlib::cout << test_pointer[5] << hwlib::endl;
    hwlib::cout << test_pointer[6] << hwlib::endl;

    hwlib::cout << test_pointer << hwlib::endl;
    for(auto it = test.begin(); it != test.end(); it++) {
        hwlib::cout << *it << hwlib::endl;
    }
    
}