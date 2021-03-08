#include <stddef.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "ram.h"
#include "spi.h"

#ifdef RUN_TESTS
#include <stdio.h>
#include <avr/pgmspace.h>
#include "serial.h"
#endif

int main()
{
    spi_init();
    ram_init();

#ifdef RUN_TESTS
    printf_P(PSTR("\e[1;1H\e[2J"));
    serial_init();
    extern void run_tests();
    run_tests();
#endif

    while (1) {
        uint8_t r = spi_swap(0xff);
        wdt_enable(WDTO_2S);
        spi_activate();
        switch (r) {
            case 0x1:
                spi_ready();
                spi_swap('H');
                spi_swap('e');
                spi_swap('l');
                spi_swap('l');
                spi_swap('o');
                spi_swap('\n');
                spi_done();
                break;
            case 0x2: {   // READ BYTE
                    uint16_t addr = spi_swap(0xff);
                    addr |= ((uint16_t) spi_swap(0xff)) << 8;
                    spi_ready();
                    uint8_t data = ram_read_byte(addr);
                    spi_swap(data);
                    spi_swap(data);
                    spi_done();
                }
                break;
            case 0x3: {    // WRITE BYTE
                    uint16_t addr = spi_swap(0xff);
                    addr |= ((uint16_t) spi_swap(0xff)) << 8;
                    uint8_t data = spi_swap(0xff);
                    uint8_t written = ram_write_byte(addr, data);
                    spi_ready();
                    spi_swap(written);
                    spi_done();
                }
                break;
            case 0x4: {    // READ BLOCK
                    uint16_t addr = spi_swap(0xff);
                    addr |= ((uint16_t) spi_swap(0xff)) << 8;
                    uint16_t sz = spi_swap(0xff);
                    sz |= ((uint16_t) spi_swap(0xff)) << 8;
                    ram_read_stream_start();
                    uint16_t sum1 = 0, sum2 = 0;
                    for (size_t i = 0; i < sz; ++i) {
                        uint8_t byte = ram_read_byte_stream(addr + i);
                        spi_ready();
                        spi_swap(byte);
                        sum1 = (sum1 + byte) % 255;
                        sum2 = (sum2 + sum1) % 255;
                        spi_done();
                    }
                    ram_read_stream_end();
                    spi_swap(sum1);
                    spi_swap(sum2);
                    spi_done();
                }
                break;
            case 0x5: {    // WRITE BLOCK
                    uint16_t addr = spi_swap(0xff);
                    addr |= ((uint16_t) spi_swap(0xff)) << 8;
                    uint16_t sz = spi_swap(0xff);
                    sz |= ((uint16_t) spi_swap(0xff)) << 8;
                    ram_write_stream_start();
                    uint16_t sum1 = 0, sum2 = 0;
                    for (size_t i = 0; i < sz; ++i) {
                        uint8_t byte = spi_swap(0xff);
                        spi_done();
                        ram_write_byte_stream(addr + i, byte);
                        spi_ready();
                        sum1 = (sum1 + byte) % 255;
                        sum2 = (sum2 + sum1) % 255;
                        _delay_us(30);
                    }
                    spi_done();
                    ram_write_stream_end();
                    spi_swap(sum1);
                    spi_swap(sum2);
                    spi_done();
                }
                break;
            case 0x6: {
                    uint8_t byte = ram_get_data();
                    spi_ready();
                    _delay_us(30);
                    spi_done();
                    spi_swap(byte);
                }
                break;
            case 0x7: {
                    uint8_t data = spi_swap(0xff);
                    ram_set_data(data);
                    spi_ready();
                    _delay_us(30);
                    spi_done();
                }
                break;
        }
        spi_deactivate();
        wdt_disable();
    }
}
