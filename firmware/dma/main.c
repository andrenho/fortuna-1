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
    /*
    sei();
    */

    printf_P(PSTR("\e[1;1H\e[2J"));
#ifdef RUN_TESTS
    extern void run_tests();
    run_tests();
#endif

    while (1) {
        uint8_t r = spi_read();
        if (r != 0xff) {
            spi_send(1);
            spi_send(2);
            spi_send(3);
            spi_send(4);
            spi_send(5);
            spi_send(6);
        }
    }
}

/*
ISR (SPI_STC_vect) {
    putchar('x');
}
*/
