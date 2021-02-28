#include "ram.hh"

#define CMD_TEST 0x1

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

void RAM::write_byte(uint16_t addr, uint8_t data)
{

}

uint8_t RAM::read_byte(uint16_t addr) const
{
    return 0;
}
