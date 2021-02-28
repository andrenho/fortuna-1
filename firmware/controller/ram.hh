#ifndef RAM_HH_
#define RAM_HH_

#include "spi.hh"

class RAM {
public:
    explicit RAM(SPI& spi): spi_(spi) {}

    const char* test();
    
    void    write_byte(uint16_t addr, uint8_t data);
    uint8_t read_byte(uint16_t addr) const;

private:
    SPI& spi_;
    char test_buf_[6] = {0};
};

#endif
