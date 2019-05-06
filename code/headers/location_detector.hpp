struct geographic_coordinate_s {
    int latitude;
    int longtitude;
};

class location_detection_c {
public:
    virtual void process()= 0;
    virtual geographic_coordinate_s get_location()= 0;
};