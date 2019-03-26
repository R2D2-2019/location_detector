#include "hwlib.hpp"

int main(void) {
  // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;
    hwlib::wait_ms(1000);
    for (;;){
        hwlib::cout << "this works via arduino";
        hwlib::wait_ms(1000);
        auto c = hwlib::target::usart_bus(hwlib::target::uart_ports::uart1);
        c.begin(9600);
        auto sda = hwlib::target::pin_oc(hwlib::target::pins::sda);
        auto scl = hwlib::target::pin_oc(hwlib::target::pins::scl);

        auto i2cBus = hwlib::i2c_bus_bit_banged_scl_sda( scl,sda );
        auto channel = hwlib::i2c_channel(i2cBus, 0x3c);

        auto display = hwlib::glcd_oled_i2c_128x64_direct( channel);

        auto font = hwlib::font_default_8x8();
        auto console = hwlib::terminal_from(display, font);

        display.clear();
        char gps_buffer[100 ];
        char cc;
        int offset = 0;
        bool complete_string = false;
        for(;;){
            if (c.char_available()){
                cc = c.getc();

                if (cc == '$'){
                    offset = 0;
                    complete_string = true;

                } else if(cc == '\n' || cc == '\r'){
                    if (complete_string) {
                        hwlib::cout << gps_buffer << "\n";
                    }
                    offset = 0;
                    complete_string = false;
                } else {
                    if (complete_string) {
                        gps_buffer[offset] = cc;
                        ++offset;
                    }
                }
            }
        }
    }
}

