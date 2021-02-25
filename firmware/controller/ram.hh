#ifndef RAM_HH_
#define RAM_HH_

#include "spi.hh"

class RAM {
public:
    RAM(SPI& spi): spi_(spi) {}

    const char* test();

private:
    SPI& spi_;
    char test_buf_[6] = {0};
};

#endif
