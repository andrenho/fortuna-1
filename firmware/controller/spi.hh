#ifndef SPI_HH_
#define SPI_HH_

#include <stddef.h>
#include <stdint.h>

#include "serial.hh"

class SPI {
public:
    enum Slave { AV, SD, DMA };

    SPI();

    void activate(Slave slave);
    void deactivate();

    uint8_t send(uint8_t byte);
    uint8_t recv();

#ifdef ENABLE_TESTS
    void set_debug_mode(bool debug_mode) { debug_mode_ = debug_mode; }
private:
    bool debug_mode_ = false;
#endif
};

#endif
