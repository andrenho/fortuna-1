#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

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
        if (r == 0x1) {
            spi_send('H');
            spi_send('e');
            spi_send('l');
            spi_send('l');
            spi_send('o');
            spi_send('\n');
        }
        spi_deactivate();
    }
}
