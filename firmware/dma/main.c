#include "serial.h"

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

int main()
{
    serial_init();
    spi_init();
    sei();

    printf("Hello.\n");

    for(;;);
}

ISR (SPI_STC_vect) {
    putchar('x');
}
