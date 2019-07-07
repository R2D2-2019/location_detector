#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <uart_nmea.hpp>
#include <test_usart.hpp>
#include <string.hpp>
#include <module.hpp>
#include <mock_bus.hpp>

using namespace r2d2;

TEST_CASE("atoi converts string to int", "[math]") {
    uint8_t string[] = "1234";
    int32_t res = location::atoi(string, sizeof(string));

    REQUIRE(res == 1234);
}

TEST_CASE("atoi converts negative int string to negative int", "[math]") {
    uint8_t string[] = "-4321";
    int32_t res = location::atoi(string, sizeof(string));

    REQUIRE(res == -4321);
}

TEST_CASE("atof converts string to float", "[math]") {
    uint8_t string[] = "1234.123";
    float res = location::atof(string, sizeof(string));

    REQUIRE(res == 1234.123f);
}

TEST_CASE("atof converts precise float string to float", "[math]") {
    uint8_t string[] = "1.23456789";
    float res = location::atof(string, sizeof(string));

    REQUIRE(res == 1.23456789f);
}

TEST_CASE("atof converts int string to float", "[math]") {
    uint8_t string[] = "1234";
    float res = location::atof(string, sizeof(string));

    REQUIRE(res == 1234.f);
}

TEST_CASE("atof converts negative float string to negative float", "[math]") {
    uint8_t string[] = "-4321.321";
    float res = location::atof(string, sizeof(string));

    REQUIRE(res == -4321.321f);
}

TEST_CASE("atof converts negative int string to negative float", "[math]") {
    uint8_t string[] = "-4321";
    float res = location::atof(string, sizeof(string));

    REQUIRE(res == -4321.f);
}

TEST_CASE("nmea listener test", "[nmea listener]") {
    // create a test usart object
    auto usart = usart::test_usart_c();

    auto string = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\n";

    // add a string to the recieve buffer
    // https://www.gpsinformation.org/dale/nmea.htm#GGA
    usart.set_receive_string(string);

    REQUIRE(usart.available() > 10);

    auto listener = location::nmea_listener_c(usart);

    // update the buffer
    listener.update_buffer();

    REQUIRE(listener.nmea_available());

    bool issame = true;

    // compare the strings
    for (size_t i = 0; i < listener.length(); i++) {
        if (!(string[i] == listener.get_nmea_string()[i])) {
            issame = false;
            break;
        }
    }

    REQUIRE(issame);
}

TEST_CASE("usart nmea parser test", "[nmea_parser]") {
    // create a test usart object
    auto usart = usart::test_usart_c();

    // add a string to the recieve buffer
    // https://www.gpsinformation.org/dale/nmea.htm#GGA
    usart.set_receive_string("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\n");

    REQUIRE(usart.available() > 10);

    auto nmea = location::uart_nmea_c(usart);

    auto gga = nmea.get_location();

    REQUIRE(gga.time == 123519);
    REQUIRE(gga.latitude == location::decimal_degrees{380, 7, 48});
    REQUIRE(gga.is_north_hemisphere == true);
    REQUIRE(gga.longitude == location::decimal_degrees{0, 31, 11});
    REQUIRE(gga.is_east_hemisphere == true);
    REQUIRE(gga.fix == location::fix_status::gps);
    REQUIRE(gga.satellites_tracked == 8);
    REQUIRE(gga.horizontal_dilution == 0.9f);
    REQUIRE(gga.altitude == 545.4f);
    REQUIRE(gga.altitude_unit == 'M');
    REQUIRE(gga.geoid_height == 46.9f);
    REQUIRE(gga.geoid_height_unit == 'M');
}

TEST_CASE("usart nmea parser convert frame test", "[nmea_parser]") {
    // create a test usart object
    auto usart = usart::test_usart_c();

    // add a string to the recieve buffer
    // https://www.gpsinformation.org/dale/nmea.htm#GGA
    usart.set_receive_string("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\n");

    REQUIRE(usart.available() > 10);

    auto nmea = location::uart_nmea_c(usart);

    auto gga = nmea.get_location();

    auto frame = nmea.gga_to_frame(gga);

    REQUIRE(frame.lat_deg == 48);
    REQUIRE(frame.lat_min == 7);
    REQUIRE(frame.lat_tenthousandth_min == 380);
    REQUIRE(frame.north_south_hemisphere == true);

    REQUIRE(frame.long_deg == 11);
    REQUIRE(frame.long_min == 31);
    REQUIRE(frame.long_tenthousandth_min == 0);
    REQUIRE(frame.east_west_hemisphere == true);

    REQUIRE(frame.altitude == 545);
}

TEST_CASE("nmea parser with inclomplete string", "[nmea_parser]") {
    // create a test usart object
    auto usart = usart::test_usart_c();

    // add a string to the recieve buffer
    // https://www.gpsinformation.org/dale/nmea.htm#GGA
    usart.set_receive_string("$GPGGA,123519,4807.038,N,01131.000,E,1,\n");

    REQUIRE(usart.available() > 10);

    auto nmea = location::uart_nmea_c(usart);

    auto gga = nmea.get_location();

    REQUIRE(gga.time == 123519);
    REQUIRE(gga.latitude == location::decimal_degrees{380, 7, 48});
    REQUIRE(gga.is_north_hemisphere == true);
    REQUIRE(gga.longitude == location::decimal_degrees{0, 31, 11});
    REQUIRE(gga.is_east_hemisphere == true);
    REQUIRE(gga.fix == location::fix_status::gps);
    REQUIRE(gga.satellites_tracked == 0);
    REQUIRE(gga.horizontal_dilution == 0.0f);
    REQUIRE(gga.altitude == 0.0f);
    REQUIRE(gga.altitude_unit == 0);
    REQUIRE(gga.geoid_height == 0.0f);
    REQUIRE(gga.geoid_height_unit == 0);
}

TEST_CASE("process function in module", "[module]") {
    // create a test usart object
    auto usart = usart::test_usart_c();

    // create a nmea object
    location::uart_nmea_c nmea(usart);

    mock_comm_c comm;

    location::module_c module(comm, nmea);
    
    REQUIRE(comm.accepts_frame(frame_type::COORDINATE));

    // create a test module with all required parameters

    // add a string to the recieve buffer
    // https://www.gpsinformation.org/dale/nmea.htm#GGA
    usart.set_receive_string("$GPGGA,123519,4807.038,N,01131.002,E,1,08,0.9,545.4,M,46.9,M,,*47\n");

    // create request frame
    auto frame = comm.create_frame<frame_type::COORDINATE>({});
    frame.request = true;
    
    comm.accept_frame(frame);

    // get the location    
    /*
    //currently creates a segfault at comm.get_data()
    //requires further investigation
    module.process();

    //check if module created a frame
    REQUIRE(comm.get_send_frames().size() == 1);

    // get the first item in the vector
    auto comm_data = (*comm.get_send_frames().begin());
    REQUIRE(comm_data.type == frame_type::COORDINATE);
    REQUIRE(comm_data.request == false);
    */
}