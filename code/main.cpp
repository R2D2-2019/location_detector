#include "hwlib.hpp"

int main(void) {
  // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;
    hwlib::wait_ms(1000);
    hwlib::wait_ms(1000);
    auto c = r2d2::usart_bus(r2d2::uart_ports::uart1(9600));


}

