#include "ram.hh"

#include <avr/pgmspace.h>
#include <stdio.h>

#include <util/delay.h>

#define CMD_TEST        0x1
#define CMD_READ_BYTE   0x2
#define CMD_WRITE_BYTE  0x3
#define CMD_READ_BLOCK  0x4
#define CMD_WRITE_BLOCK 0x5

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
    _delay_us(100);
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
    _delay_us(100);
    uint8_t r = spi_.recv();
    spi_.deactivate();
    return r;
}

bool RAM::read_block(uint16_t addr, uint16_t sz, RAM::ReadFunc read_func, void* data) const
{
    spi_.activate(SPI::DMA);
    spi_.send(CMD_READ_BLOCK);
    spi_.send(addr & 0xff);
    spi_.send(addr >> 8);
    spi_.send(sz & 0xff);
    spi_.send(sz >> 8);
    _delay_ms(100);
    uint16_t sum1 = 0, sum2 = 0;
    for (uint16_t a = addr; a < (addr + sz); ++a) {
        uint8_t byte = spi_.recv();
        read_func(a, byte, data);
        sum1 = (sum1 + byte) % 255;
        sum2 = (sum2 + sum1) % 255;
    }
    _delay_ms(10);
    uint8_t csum1 = spi_.recv();
    uint8_t csum2 = spi_.recv();
    spi_.deactivate();
    return sum1 == csum1 && sum2 == csum2;
}

bool RAM::write_block(uint16_t addr, uint16_t sz, RAM::WriteFunc write_func, void* data)
{
    spi_.activate(SPI::DMA);
    spi_.send(CMD_WRITE_BLOCK);
    spi_.send(addr & 0xff);
    spi_.send(addr >> 8);
    spi_.send(sz & 0xff);
    spi_.send(sz >> 8);
    uint16_t sum1 = 0, sum2 = 0;
    for (uint16_t a = addr; a < (addr + sz); ++a) {
        uint8_t byte = write_func(a, data);
        spi_.send(byte);
        sum1 = (sum1 + byte) % 255;
        sum2 = (sum2 + sum1) % 255;
    }
    _delay_ms(100);
    uint8_t csum1 = spi_.recv();
    uint8_t csum2 = spi_.recv();
    spi_.deactivate();
    return sum1 == csum1 && sum2 == csum2;
}
