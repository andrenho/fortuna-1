#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

void    spi_init();

void    spi_activate();
void    spi_deactivate();

uint8_t spi_swap(uint8_t byte);

#endif
