#include "fortuna1.hh"

#include <avr/interrupt.h>
#include <avr/wdt.h>

void Fortuna1::hard_reset(Buffer& buffer)
{
    z80_.powerdown();
    sdcard_.initialize();
    
    sdcard_.read_page(0, buffer);
    ram_.write_block(0, 512, [](uint16_t idx, void* data) { return ((uint8_t*) data)[idx]; }, buffer.data);
    
    z80_.startup();
}

void Fortuna1::soft_reset()
{
    z80_.powerdown();
    sdcard_.initialize();
    z80_.startup();
}

[[noreturn]] void Fortuna1::system_reset()
{
    cli();
    wdt_enable(WDTO_15MS);
    while(true) {}
}

uint8_t Fortuna1::write_byte(uint16_t addr, uint8_t data)
{
    z80_.request_bus();
    return ram_.write_byte(addr, data);
}

uint8_t Fortuna1::read_byte(uint16_t addr) const
{
    z80_.request_bus();
    return ram_.read_byte(addr);
}

bool Fortuna1::read_block(uint16_t addr, uint16_t sz, RAM::ReadFunc read_func, void* data) const
{
    z80_.request_bus();
    return ram_.read_block(addr, sz, read_func, data);
}

bool Fortuna1::write_block(uint16_t addr, uint16_t sz, RAM::WriteFunc write_func, void* data)
{
    z80_.request_bus();
    return ram_.write_block(addr, sz, write_func, data);
}

