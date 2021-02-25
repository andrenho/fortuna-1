#ifndef SPI_HH_
#define SPI_HH_

#include <stdint.h>

class SPI {
public:
    enum Slave { AV, SD, DMA };

    SPI();

    void activate(Slave slave);
    void deactivate();

    uint8_t send(uint8_t byte);
    uint8_t recv();
};

#endif
