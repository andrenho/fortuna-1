#include "spi.hh"

#include <avr/io.h>

#include "io.hh"

SPI::SPI()
{
    set_MOSI(0);
    set_SCLK(0);

    // enable SPI, set as MASTER, clock to fosc/128
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
}

void SPI::activate(Slave slave)
{
    switch (slave) {
        case AV:  set_AVCS(0);  break;
        case SD:  set_SDCS(0);  break;
        case DMA: set_DMACS(0); break;
    }
}

void SPI::deactivate()
{
    set_AVCS(1);
    set_SDCS(1);
    set_DMACS(1);
}

uint8_t SPI::send(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

uint8_t SPI::recv()
{
    return send(0xff);
}
