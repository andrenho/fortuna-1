#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "ram.h"
#include "serial.h"
#include "spi.h"

static uint16_t checksum(uint16_t sz)
{
    uint16_t sum1 = 0, sum2 = 0;
    for (size_t i = 0; i < sz; ++i) {
        sum1 = (sum1 + buffer[i]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }
    return (sum2 << 8) | sum1;
}

int main()
{
    serial_init();
    spi_init();
    ram_init();

    printf_P(PSTR("\e[1;1H\e[2J"));
#ifdef RUN_TESTS
    extern void run_tests();
    run_tests();
#endif

    while (1) {
        uint8_t r = spi_swap(0xff);
        spi_activate();
        switch (r) {
            case 0x1:
                spi_swap('H');
                spi_swap('e');
                spi_swap('l');
                spi_swap('l');
                spi_swap('o');
                spi_swap('\n');
                break;
            case 0x2: {
                    uint16_t addr = spi_swap(0xff);
                    addr |= ((uint16_t) spi_swap(0xff)) << 8;
                    spi_swap(0xfe);
                    spi_swap(ram_read_byte(addr));
                }
                break;
            case 0x3: {
                    uint16_t addr = spi_swap(0xff);
                    addr |= ((uint16_t) spi_swap(0xff)) << 8;
                    uint8_t data = spi_swap(0xff);
                    uint8_t written = ram_write_byte(addr, data);
                    spi_swap(0xfe);
                    spi_swap(written);
                }
                break;
            case 0x4: {
                    uint16_t addr = spi_swap(0xff);
                    addr |= ((uint16_t) spi_swap(0xff)) << 8;
                    uint16_t sz = spi_swap(0xff);
                    sz |= ((uint16_t) spi_swap(0xff)) << 8;
                    ram_read_buffer(addr, sz);
                    spi_swap(0xfe);
                    for (size_t i = 0; i < sz; ++i)
                        spi_swap(buffer[i]);
                    uint16_t chk = checksum(sz);
                    spi_swap(chk & 0xff);
                    spi_swap(chk >> 8);
                }
                break;
            case 0x5: {
                    uint16_t addr = spi_swap(0xff);
                    addr |= ((uint16_t) spi_swap(0xff)) << 8;
                    uint16_t sz = spi_swap(0xff);
                    sz |= ((uint16_t) spi_swap(0xff)) << 8;
                    for (size_t i = 0; i < sz; ++i)
                        buffer[i] = spi_swap(0xff);
                    ram_write_buffer(addr, sz);
                    spi_swap(0xfe);
                    uint16_t chk = checksum(sz);
                    spi_swap(chk & 0xff);
                    spi_swap(chk >> 8);
                }
                break;
            /*
            case 0x6:
                spi_swap(ram_get_data());
                break;
            case 0x7: {
                    uint8_t data = spi_swap(0xff);
                    ram_set_data(data);
                    spi_swap(data);
                }
                break;
            default:
                spi_swap(0xfe);
                */
        }
        spi_deactivate();
    }
}
