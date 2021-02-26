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
    sei();

    printf_P(PSTR("\e[1;1H\e[2J"));
#ifdef RUN_TESTS
    extern void run_tests();
    run_tests();
#endif

    for(;;);
}

ISR (SPI_STC_vect) {
    putchar('x');
}
