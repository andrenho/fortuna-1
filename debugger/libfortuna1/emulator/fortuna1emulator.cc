#include "fortuna1emulator.hh"

#include <iostream>

void Fortuna1Emulator::reset()
{
    ResetZ80(&z80_);
    auto boot = sdcard_read(0);
    ram_write_buffer(0, std::vector<uint8_t>(boot.begin(), boot.end()));
}

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

std::array<uint8_t, 512> Fortuna1Emulator::sdcard_read(uint32_t block)
{
    if (!sd_image_stream_.has_value())
        throw std::runtime_error("A SD card image is not defined in `sdcard_read`.");
    sd_image_stream_->seekp(block * 512, std::ios_base::beg);
    std::array<uint8_t, 512> data {0};
    sd_image_stream_->read(reinterpret_cast<char*>(data.data()), 512);
    return data;
}

