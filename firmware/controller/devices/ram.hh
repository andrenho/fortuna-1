#ifndef RAM_HH_
#define RAM_HH_

#include "protocol/spi.hh"

class RAM {
public:
    using ReadFunc = void (*)(uint16_t idx, uint8_t byte, void* data);
    using WriteFunc = uint8_t (*)(uint16_t idx, void* data);
    
    explicit RAM(SPI& spi): spi_(spi) {}

    const char* test();
    
    uint8_t write_byte(uint16_t addr, uint8_t data);
    uint8_t read_byte(uint16_t addr) const;
    
    bool read_block(uint16_t addr, uint16_t sz, ReadFunc read_func, void* data = nullptr) const;
    bool write_block(uint16_t addr, uint16_t sz, WriteFunc write_func, void* data = nullptr);
    
    uint8_t data_bus() const;
    void    set_data_bus(uint8_t data);

    SPI& spi() const { return spi_; }

private:
    SPI& spi_;
    char test_buf_[6] = {0};
};

#endif
