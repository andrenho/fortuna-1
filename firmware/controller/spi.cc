#include "spi.hh"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "io.hh"

SPI::SPI()
{
    set_MOSI_as_output();
    set_SCLK_as_output();
    set_DMA_CS_as_input();
    set_MOSI(0);
    set_SCLK(0);

    // enable SPI, set as MASTER, clock to fosc/128
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);

    deactivate();
}

void SPI::activate(Slave slave)
{
    set_MOSI_as_output();
    set_SCLK_as_output();
    set_MOSI(0);
    set_SCLK(0);
    switch (slave) {
        case AV:
            set_AVCS(0);
            break;
        case SD:
            set_SDCS(0);
            break;
        case DMA:
            set_DMACS(0);
            break;
    }
}

void SPI::deactivate()
{
    set_AVCS(1);
    set_SDCS(1);
    set_DMACS(1);
    set_MOSI_as_input();  // release SPI lines if they need to be used to program the DMA
    set_SCLK_as_input();
}

uint8_t SPI::send(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & (1 << SPIF)));
    uint8_t r = SPDR;
#ifdef ENABLE_TESTS
    if (debug_mode_)
        printf("\e[0;36m%02X\e[0m.\e[0;33m%02X \e[0m ", byte, r);
#endif
    return r;
}

uint8_t SPI::recv()
{
    _delay_us(300);
    return send(0xff);
}

void SPI::wait_dma_cs() const
{
    while (get_DMA_CS() == 0);   // TODO - for how long?
}
