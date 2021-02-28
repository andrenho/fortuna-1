#include "fortuna1emulator.hh"

#include <iostream>

void Fortuna1Emulator::test_debug_messages() const
{
    for (int i = 0; i < 3; ++i)
        std::cout << "Debug message " << i << " from emulator.\n";
}

std::string Fortuna1Emulator::test_dma() const
{
    return "Hello";
}

void Fortuna1Emulator::ram_write_byte(uint16_t addr, uint8_t data)
{
    ram_[addr] = data;
}

uint8_t Fortuna1Emulator::ram_read_byte(uint16_t addr) const
{
    return ram_[addr];
}
