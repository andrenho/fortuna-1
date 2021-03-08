#include "fortuna1.hh"

void Fortuna1::reset(Buffer& buffer)
{
    z80_.powerdown();
    sdcard_.initialize();
    
    sdcard_.read_page(0, buffer);
    ram_.write_block(0, 512, [](uint16_t idx, void* data) { return ((uint8_t*) data)[idx]; }, buffer.data);
    
    z80_.startup();
}
