#include "ram.hh"

#define CMD_TEST       0x1
#define CMD_READ_BYTE  0x2
#define CMD_WRITE_BYTE 0x3

const char* RAM::test()
{
    spi_.activate(SPI::DMA);
    spi_.send(CMD_TEST);
    for (int i = 0; i < 6; ++i)
        test_buf_[i] = spi_.recv();
    spi_.deactivate();
    test_buf_[5] = 0;  // ensures no issues with end of string
    return test_buf_;
}

uint8_t RAM::write_byte(uint16_t addr, uint8_t data)
{
    spi_.activate(SPI::DMA);
    spi_.send(CMD_WRITE_BYTE);
    spi_.send(addr & 0xff);
    spi_.send(addr >> 8);
    spi_.send(data);
    uint8_t r = spi_.recv();
    spi_.deactivate();
    return r;
}

uint8_t RAM::read_byte(uint16_t addr) const
{
    spi_.activate(SPI::DMA);
    spi_.send(CMD_READ_BYTE);
    spi_.send(addr & 0xff);
    spi_.send(addr >> 8);
    uint8_t r = spi_.recv();
    spi_.deactivate();
    return r;
}
