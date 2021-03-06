#include <stddef.h>
#include "ram.h"
#include "spi.h"

#ifdef RUN_TESTS
#include <stdio.h>
#include <avr/pgmspace.h>
#include "serial.h"
#endif

static uint16_t checksum(uint16_t sz, uint8_t* buffer)
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
    uint8_t buffer[256];

    spi_init();
    ram_init();

#ifdef RUN_TESTS
    printf_P(PSTR("\e[1;1H\e[2J"));
    serial_init();
    extern void run_tests();
    run_tests();
#endif

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (1) {
        uint8_t r = spi_swap(0xff);
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
                    ram_read_buffer(addr, sz, buffer);
                    spi_ready();
                    for (size_t i = 0; i < sz; ++i)
                        spi_swap(buffer[i]);
                    uint16_t chk = checksum(sz, buffer);
                    spi_swap(chk & 0xff);
                    spi_swap(chk >> 8);
                    spi_done();
                }
                break;
            case 0x5: {    // WRITE BLOCK
                uint16_t addr = spi_swap(0xff);
                addr |= ((uint16_t) spi_swap(0xff)) << 8;
                uint16_t sz = spi_swap(0xff);
                sz |= ((uint16_t) spi_swap(0xff)) << 8;
                for (size_t i = 0; i < sz; ++i)
                    buffer[i] = spi_swap(0xff);
                uint16_t remote_chk = spi_swap(0xff);
                remote_chk |= ((uint16_t) spi_swap(0xff)) << 8;
                uint16_t chk = checksum(sz, buffer);
                ram_write_buffer(addr, sz, buffer);
                spi_ready();
                spi_swap(remote_chk == chk ? 0 : 1);
                spi_swap(chk & 0xff);
                spi_swap(chk >> 8);
                spi_done();
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
#pragma clang diagnostic pop
}
