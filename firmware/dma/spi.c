#include "spi.h"

#include <avr/io.h>

void spi_init()
{
    DDRB &= _BV(PORTB4);  // SS (input)
    DDRB &= _BV(PORTB5);  // MOSI (input)
    DDRB &= _BV(PORTB6);  // MISO (output - only when in use)
    DDRB &= _BV(PORTB7);  // SCLK (input)

    // enable SPI, set as SLAVE, clock to fosc/128 (doesn't really matter), enable interrupts
    SPCR = (1 << SPE) | (1 << SPR1) | (1 << SPR0) /* | (1 << SPIE) */;
}

uint8_t spi_read()
{
    while(!(SPSR & (1<<SPIF)));
    return SPDR; 
}

void spi_send(uint8_t byte)
{
    SPDR = byte;
}
