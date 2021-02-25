#ifndef RAM_HH_
#define RAM_HH_

#include "spi.hh"

class RAM {
public:
    RAM(SPI& spi): spi_(spi) {}

private:
    SPI& spi_;
};

#endif
