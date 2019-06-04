#include <hwlib.hpp>
#include <module.hpp>
#include <uart_nmea.hpp>
#include <hardware_usart.hpp>
#include <comm.hpp>

int main(void) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;
    hwlib::wait_ms(1000);
    hwlib::cout << "this works via arduino" << hwlib::endl;

    r2d2::comm_c comm;

    r2d2::usart::hardware_usart_c<r2d2::usart::usart0> usart(9600);

    r2d2::location::uart_nmea_c nmea(usart);

    r2d2::location::module_c module(comm, nmea);

    while(true) {
        module.process();
    }
}