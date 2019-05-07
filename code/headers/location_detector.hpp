#include "hwlib.hpp"

struct geographic_coordinate_s {
    uint32_t latitude;
    uint32_t longtitude;
    uint8_t north_east;
    int16_t altitude;
};

class location_detection_c {
public:
    virtual void process() = 0;

protected:
    virtual geographic_coordinate_s get_location() = 0;
    geographic_coordinate_s compress_coordinate();
};
