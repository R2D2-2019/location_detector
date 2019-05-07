#include "hwlib.hpp"

struct geographic_coordinate_s {
    uint32_t latitude;
    unint32_t longtitude;
    uint8_t NESW;
    int altitude;
};

class location_detection_c {
public:
    virtual void process() = 0;
    virtual geographic_coordinate_s get_location() = 0;
};
