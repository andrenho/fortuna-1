#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "ram.h"
#include "serial.h"
#include "spi.h"

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
        uint8_t r = spi_read();
        spi_activate();
        switch (r) {
            case 0x1:
                spi_send('H');
                spi_send('e');
                spi_send('l');
                spi_send('l');
                spi_send('o');
                spi_send('\n');
                break;
            case 0x2: {
                    uint16_t addr = spi_read();
                    addr |= ((uint16_t) spi_read()) << 8;
                    spi_send(ram_read_byte(addr));
                }
                break;
            case 0x3: {
                    uint16_t addr = spi_read();
                    addr |= ((uint16_t) spi_read()) << 8;
                    uint8_t data = spi_read();
                    ram_write_byte(addr, data);
                    spi_send(data);
                }
                break;

        }
        spi_deactivate();
    }
}
