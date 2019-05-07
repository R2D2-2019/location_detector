struct geographic_coordinate_s {
    signed int latitude;
    char NS;
    signed int longtitude;
    char EW;
    int altitude;
};

class location_detection_c {
public:
    virtual void process() = 0;
    virtual geographic_coordinate_s get_location() = 0;
};