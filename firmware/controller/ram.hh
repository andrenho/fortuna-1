#ifndef RAM_HH_
#define RAM_HH_

#include "spi.hh"

class RAM {
    using ReadFunc = void (*)(uint16_t addr, uint8_t byte, void* data);
    using WriteFunc = uint8_t (*)(uint16_t addr, void* data);
public:
    explicit RAM(SPI& spi): spi_(spi) {}

    const char* test();
    
    uint8_t write_byte(uint16_t addr, uint8_t data);
    uint8_t read_byte(uint16_t addr) const;
    
    bool read_block(uint16_t addr, uint16_t sz, ReadFunc read_func, void* data = nullptr) const;
    bool write_block(uint16_t addr, uint16_t sz, WriteFunc write_func, void* data = nullptr);

private:
    SPI& spi_;
    char test_buf_[6] = {0};
};

#endif
