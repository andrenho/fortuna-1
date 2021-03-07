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

void Fortuna1Emulator::ram_write_buffer(uint16_t addr, std::vector<uint8_t> const& bytes)
{
    size_t i = 0;
    for (uint16_t a = addr; a < (uint16_t) (addr + bytes.size()); ++a)
        ram_[a] = bytes[i++];
}

std::vector<uint8_t> Fortuna1Emulator::ram_read_buffer(uint16_t addr, uint16_t sz) const
{
    std::vector<uint8_t> r;
    for (uint16_t a = addr; a < (uint16_t) (addr + sz); ++a)
        r.push_back(ram_[a]);
    return r;
}

SDCardStatus Fortuna1Emulator::sdcard_status() const
{
    return { sd_card_.last_stage(), sd_card_.last_response() };
}

void Fortuna1Emulator::sdcard_set_image(std::string const& filename)
{
    sd_image_stream_.emplace(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!sd_image_stream_->is_open())
        throw std::runtime_error("The file " + filename + " could not be open.");
}
