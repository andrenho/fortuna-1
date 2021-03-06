#include "spi.h"

#include <avr/io.h>

void spi_init()
{
    DDRB &= ~_BV(PORTB4);  // SS (input)
    DDRB &= ~_BV(PORTB5);  // MOSI (input)
    DDRB &= ~_BV(PORTB7);  // SCLK (input)
    DDRD |= _BV(PORTD1);   // DMA_CS
    PORTD |= _BV(PORTD1);

    // enable SPI, set as SLAVE, clock to fosc/128 (doesn't really matter), enable interrupts
    SPCR = (1 << SPE) | (1 << SPR1) | (1 << SPR0) /* | (1 << SPIE) */;
}

void spi_ready()
{
    PORTD &= ~_BV(PORTD1);
}

void spi_done()
{
    PORTD |= _BV(PORTD1);
}

void spi_activate()
{
    DDRB |= _BV(PORTB6);  // MISO (output - only when in use)
}

void spi_deactivate()
{
    DDRB &= ~_BV(PORTB6);
}

uint8_t spi_swap(uint8_t byte)
{
    SPDR = byte;
    while(!(SPSR & (1<<SPIF)));
    return SPDR;
}
