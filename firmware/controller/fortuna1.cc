#include "fortuna1.hh"

ResetStatus Fortuna1::hard_reset(Buffer& buffer)
{
    bool sdcard_read_ok = false;
    bool bootable = false;

    z80_.powerdown();

    bool sdcard_init_ok = sdcard_.initialize();
    if (sdcard_init_ok)
        sdcard_read_ok = sdcard_.read_page(0, buffer);

    if (sdcard_read_ok) {
        bootable = (buffer.data[0x1fe] == 0x55 && buffer.data[0x1ff] == 0xaa);
        if (bootable)
            ram_.write_block(0, 512, [](uint16_t idx, void* data) { return ((uint8_t*) data)[idx]; }, buffer.data);
    }
    
    z80_.startup();

    if (!sdcard_init_ok)
        return ResetStatus::SDCardInitError;
    else if (!sdcard_read_ok)
        return ResetStatus::SDCardReadError;
    else if (!bootable)
        return ResetStatus::SDCardNotBootable;
    return ResetStatus::Ok;
}

ResetStatus Fortuna1::soft_reset()
{
    z80_.powerdown();
    sdcard_.initialize();
    z80_.startup();
    return ResetStatus::Ok;
}

void Fortuna1::system_reset()
{
    void (*f)() = 0;
    f();
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

