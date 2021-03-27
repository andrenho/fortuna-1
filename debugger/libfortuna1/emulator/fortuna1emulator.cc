#include "fortuna1emulator.hh"

#include <iostream>

#include "../libf1comm/fortuna1def.hh"

Fortuna1Emulator* emulator = nullptr;

Fortuna1Emulator::Fortuna1Emulator()
{
    z80_.User = this;
    emulator = this;
}

void Fortuna1Emulator::basic_reset()
{
    sdcard_initialize();
    ResetZ80(&z80_);
    cycle_count_ = 0;
    last_printed_char_ = 0;
    last_keypress_ = 0;
    interrupt_ = false;
}

ResetStatus Fortuna1Emulator::hard_reset()
{
    basic_reset();
    try {
        auto boot = sdcard_read(0);
        ram_write_buffer(0, std::vector<uint8_t>(boot.begin(), boot.end()));
        return (boot[0x1fe] == 0x55 && boot[0x1ff] == 0xaa) ? ResetStatus::Ok : ResetStatus::SDCardNotBootable;
    } catch (std::runtime_error&) {
        return ResetStatus::SDCardInitError;
    }
}

ResetStatus Fortuna1Emulator::soft_reset()
{
    basic_reset();
    if (sd_image_stream_.has_value())
        return ResetStatus::Ok;
    else
        return ResetStatus::SDCardInitError;
}

void Fortuna1Emulator::system_reset()
{
    hard_reset();
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

Z80_Info Fortuna1Emulator::z80_info() const
{
    return { cycle_count_, z80_.PC.W, last_printed_char_ };
}

Z80_Info Fortuna1Emulator::z80_step()
{
    last_printed_char_ = 0;
    RunZ80(&z80_);
    return z80_info();
}

Z80_Info Fortuna1Emulator::z80_nmi()
{
    last_printed_char_ = 0;
    nmi_ = true;
    RunZ80(&z80_);
    return z80_info();
}

void Fortuna1Emulator::keypress(uint16_t key)
{
    last_keypress_ = key;
    interrupt_ = true;
}

void Fortuna1Emulator::sdcard_initialize()
{
    ram_[SD_STATUS] = 0b10;
}

SDCardStatus Fortuna1Emulator::sdcard_status() const
{
    return { sd_card_.last_stage(), sd_card_.last_response() };
}

void Fortuna1Emulator::sdcard_set_image(std::string const& filename)
{
    sd_image_stream_.emplace(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!sd_image_stream_->is_open()) {
        ram_[SD_STATUS] = 0b11;
        throw std::runtime_error("The file " + filename + " could not be open.");
    }
}

std::array<uint8_t, 512> Fortuna1Emulator::sdcard_read(uint32_t block)
{
    if (!sd_image_stream_.has_value()) {
        ram_[SD_STATUS] = 0b101;
        throw std::runtime_error("A SD card image is not defined in `sdcard_read`.");
    }
    try {
        std::array<uint8_t, 512> data {0};
        sd_image_stream_->seekp(block * 512, std::ios_base::beg);
        sd_image_stream_->read(reinterpret_cast<char*>(data.data()), 512);
        ram_[SD_STATUS] = 0b100;
        return data;
    } catch (std::ios::failure& e) {
        ram_[SD_STATUS] = 0b101;
        throw;
    }
}

void Fortuna1Emulator::sdcard_write(uint32_t block, std::array<uint8_t, 512> const& data)
{
    if (!sd_image_stream_.has_value()) {
        ram_[SD_STATUS] = 0b1001;
        throw std::runtime_error("A SD card image is not defined in `sdcard_read`.");
    }
    try {
        sd_image_stream_->seekp(block * 512, std::ios_base::beg);
        sd_image_stream_->write(reinterpret_cast<const char*>(data.data()), 512);
        ram_[SD_STATUS] = 0b1000;
    } catch (std::ios::failure& e) {
        ram_[SD_STATUS] = 0b1001;
        throw;
    }
}

std::vector<uint16_t> Fortuna1Emulator::list_breakpoints() const
{
    return std::vector<uint16_t>(breakpoints_.begin(), breakpoints_.end());
}

std::vector<uint16_t> Fortuna1Emulator::add_breakpoint(uint16_t address)
{
    breakpoints_.insert(address);
    return std::vector<uint16_t>(breakpoints_.begin(), breakpoints_.end());
}

std::vector<uint16_t> Fortuna1Emulator::remove_breakpoint(uint16_t address)
{
    breakpoints_.erase(address);
    return std::vector<uint16_t>(breakpoints_.begin(), breakpoints_.end());
}

std::vector<uint16_t> Fortuna1Emulator::remove_all_breakpoints(uint16_t address)
{
    breakpoints_.clear();
    return std::vector<uint16_t>(breakpoints_.begin(), breakpoints_.end());
}
