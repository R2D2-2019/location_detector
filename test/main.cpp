#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <uart_nmea_c.hpp>
#include <test_usart.hpp>
#include <string.hpp>

using namespace r2d2;

TEST_CASE("atoi converts string to int", "[math]") {
    uint8_t string[] = "1234";
    int32_t res = string::atoi(string, sizeof(string));

    REQUIRE(res == 1234);
}

TEST_CASE("atoi converts negative int string to negative int", "[math]") {
    uint8_t string[] = "-4321";
    int32_t res = string::atoi(string, sizeof(string));

    REQUIRE(res == -4321);
}

TEST_CASE("atof converts string to float", "[math]") {
    uint8_t string[] = "1234.123";
    float res = string::atof(string, sizeof(string));

    REQUIRE(res == 1234.123f);
}

TEST_CASE("atof converts precise float string to float", "[math]") {
    uint8_t string[] = "1.23456789";
    float res = string::atof(string, sizeof(string));

    REQUIRE(res == 1.23456789f);
}

TEST_CASE("atof converts int string to float", "[math]") {
    uint8_t string[] = "1234";
    float res = string::atof(string, sizeof(string));

    REQUIRE(res == 1234.f);
}

TEST_CASE("atof converts negative float string to negative float", "[math]") {
    uint8_t string[] = "-4321.321";
    float res = string::atof(string, sizeof(string));

    REQUIRE(res == -4321.321f);
}

TEST_CASE("atof converts negative int string to negative float", "[math]") {
    uint8_t string[] = "-4321";
    float res = string::atof(string, sizeof(string));

    REQUIRE(res == -4321.f);
}

TEST_CASE("usart nmea parser test altitude", "[nmea_parser]") {
    // create a test usart object
    auto usart = usart::test_usart_c();

    // add a string to the recieve buffer
    // https://www.gpsinformation.org/dale/nmea.htm#GGA
    usart.set_receive_string("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\n");

    REQUIRE(usart.available() > 10);

    auto nmea = location::uart_nmea_c(usart);

    auto frame = nmea.get_location();

    REQUIRE(frame.lat_deg == 48);
    REQUIRE(frame.lat_sec == 7);
    REQUIRE(frame.lat_thousandth_sec == 38);
    REQUIRE(frame.north_south_hemisphere == true);

    REQUIRE(frame.long_deg == 11);
    REQUIRE(frame.long_sec == 31);
    REQUIRE(frame.long_thousandth_sec == 0);
    REQUIRE(frame.east_west_hemisphere == true);

    REQUIRE(frame.altitude == 545);
}