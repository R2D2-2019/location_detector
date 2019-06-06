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
    REQUIRE(gga.latitude == 4807.038f);
    REQUIRE(gga.north_south_hemisphere == 'N');
    REQUIRE(gga.longitude == 01131.000f);
    REQUIRE(gga.east_west_hemisphere == 'E');
    REQUIRE(gga.fix_quality == 1);
    REQUIRE(gga.satellites_tracked == 8);
    REQUIRE(gga.horizontal_dilution == 0.9f);
    REQUIRE(gga.altitude == 545.4f);
    REQUIRE(gga.altitude_measurement == 'M');
    REQUIRE(gga.geoid_height == 46.9f);
    REQUIRE(gga.geoid_height_measurement == 'M');
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
    REQUIRE(gga.latitude == 4807.038f);
    REQUIRE(gga.north_south_hemisphere == 'N');
    REQUIRE(gga.longitude == 01131.000f);
    REQUIRE(gga.east_west_hemisphere == 'E');
    REQUIRE(gga.fix_quality == 1);
    REQUIRE(gga.satellites_tracked == 0);
    REQUIRE(gga.horizontal_dilution == 0.0f);
    REQUIRE(gga.altitude == 0.0f);
    REQUIRE(gga.altitude_measurement == 0);
    REQUIRE(gga.geoid_height == 0.0f);
    REQUIRE(gga.geoid_height_measurement == 0);
}

TEST_CASE("process function in module", "[module]") {
    // create a test usart object
    auto usart = usart::test_usart_c();
    r2d2::location::uart_nmea_c nmea(usart);
    r2d2::mock_comm_c comm;
    r2d2::location::module_c module(comm, nmea);

    const auto frame = comm.create_frame<r2d2::frame_type::COORDINATE>({true});
    comm.accept_frame(frame);

    // add a string to the recieve buffer
    // https://www.gpsinformation.org/dale/nmea.htm#GGA
    usart.set_receive_string("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\n");

    REQUIRE(usart.available() > 10);

    auto comm_data = comm.get_data();
    REQUIRE(comm_data.type == frame_type::COORDINATE);
    auto gga = comm_data.as_frame_type<frame_type::COORDINATE>();
    /*REQUIRE(comm_gga.time == 123519);
    REQUIRE(comm_gga.latitude == 4807.038f);
    REQUIRE(comm_gga.longitude == 01131.000f);
    REQUIRE(comm_gga.fix_quality == 1);
    REQUIRE(comm_gga.satellites_tracked == 8);
    REQUIRE(comm_gga.horizontal_dilution == 0.9f);
    REQUIRE(comm_gga.altitude == 545);
    REQUIRE(comm_gga.altitude_measurement == 'M');
    REQUIRE(comm_gga.geoid_height == 46.9f);
    REQUIRE(comm_gga.geoid_height_measurement == 'M');*/

    REQUIRE(gga.altitude == 0);
    REQUIRE(gga.long_tenthousandth_min == 0);    
    REQUIRE(gga.lat_tenthousandth_min == 0);
    REQUIRE(gga.lat_deg == 0);
    REQUIRE(gga.lat_min == 0);
    REQUIRE(gga.long_deg == 0);
    REQUIRE(gga.long_min == 0);
    REQUIRE(gga.north_south_hemisphere == true);
    REQUIRE(gga.east_west_hemisphere == true);
}