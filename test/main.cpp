#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <uart_nmea_c.hpp>

using namespace r2d2;

TEST_CASE("atoi converts string to int", "[math]") {
    uint8_t string[] = "1234";
    int32_t res = location_detector::atoi(string, sizeof(string));

    REQUIRE(res == 1234);
}

TEST_CASE("atoi converts negative int string to negative int", "[math]") {
    uint8_t string[] = "-4321";
    int32_t res = location_detector::atoi(string, sizeof(string));

    REQUIRE(res == -4321);
}

TEST_CASE("atof converts string to float", "[math]") {
    uint8_t string[] = "1234.123";
    float res = location_detector::atof(string, sizeof(string));

    REQUIRE(res == 1234.123f);
}

TEST_CASE("atof converts precise float string to float", "[math]") {
    uint8_t string[] = "1.23456789";
    float res = location_detector::atof(string, sizeof(string));

    REQUIRE(res == 1.23456789f);
}

TEST_CASE("atof converts int string to float", "[math]") {
    uint8_t string[] = "1234";
    float res = location_detector::atof(string, sizeof(string));

    REQUIRE(res == 1234.f);
}

TEST_CASE("atof converts negative float string to negative float", "[math]") {
    uint8_t string[] = "-4321.321";
    float res = location_detector::atof(string, sizeof(string));

    REQUIRE(res == -4321.321f);
}

TEST_CASE("atof converts negative int string to negative float", "[math]") {
    uint8_t string[] = "-4321";
    float res = location_detector::atof(string, sizeof(string));

    REQUIRE(res == -4321.f);
}
