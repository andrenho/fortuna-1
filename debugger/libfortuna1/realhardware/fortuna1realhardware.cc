#include "fortuna1realhardware.hh"

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

#include "../replyexception.hh"
#include "../libf1comm/messages/request.hh"

Fortuna1RealHardware::Fortuna1RealHardware(std::string const& serial_port)
    : serial_(serial_port.c_str())
{
}

size_t Fortuna1RealHardware::free_mem() const
{
    buffer_.sz = 0;
    Request request(MessageType::FreeMem, buffer_);
    return serial_.request(request, buffer_).free_mem;
}

void Fortuna1RealHardware::test_debug_messages() const
{
    buffer_.sz = 0;
    Request request(MessageType::TestDebug, buffer_);
    serial_.request(request, buffer_);
}

std::string Fortuna1RealHardware::test_dma() const
{
    buffer_.sz = 0;
    Request request(MessageType::TestDMA, buffer_);
    serial_.request(request, buffer_);
    return (const char*) buffer_.data;
}

void Fortuna1RealHardware::ram_write_byte(uint16_t addr, uint8_t data)
{
    buffer_.sz = 0;
    Request request(MessageType::RamWriteByte, buffer_);
    request.ram_request.address = addr;
    request.ram_request.byte = data;
    uint8_t response = serial_.request(request, buffer_).ram_byte;
    if (response != data)
        throw ReplyException("The byte sent to memory is not the same one returned by DMA.");
}

uint8_t Fortuna1RealHardware::ram_read_byte(uint16_t addr) const
{
    buffer_.sz = 0;
    Request request(MessageType::RamReadByte, buffer_);
    request.ram_request.address = addr;
    return serial_.request(request, buffer_).ram_byte;
}

void Fortuna1RealHardware::ram_write_buffer(uint16_t addr, std::vector<uint8_t> const& bytes)
{
    for (size_t i = 0; i < bytes.size(); ++i)
        buffer_.data[i] = bytes.at(i);
    buffer_.sz = bytes.size();
    Request request(MessageType::RamWriteBlock, buffer_);
    request.ram_request.address = addr;
    request.ram_request.size = bytes.size();
    serial_.request(request, buffer_);
}

std::vector<uint8_t> Fortuna1RealHardware::ram_read_buffer(uint16_t addr, uint16_t sz) const
{
    Request request(MessageType::RamReadBlock, buffer_);
    request.ram_request.address = addr;
    request.ram_request.size = sz;
    serial_.request(request, buffer_);
    std::vector<uint8_t> vv;
    for (size_t i = 0; i < buffer_.sz; ++i)
        vv.push_back(buffer_.data[i]);
    return vv;
}

SDCardStatus Fortuna1RealHardware::sdcard_status() const
{
    buffer_.sz = 0;
    Request request(MessageType::SDCard_Status, buffer_);
    return serial_.request(request, buffer_).sd_status;
}

std::array<uint8_t, 512> Fortuna1RealHardware::sdcard_read(uint32_t block)
{
    Request request(MessageType::SDCard_Read, buffer_);
    request.sdcard_block = block;
    serial_.request(request, buffer_);
    std::array<uint8_t, 512> data {};
    std::copy(std::begin(buffer_.data), std::end(buffer_.data), data.begin());
    return data;
}

ResetStatus Fortuna1RealHardware::hard_reset()
{
    buffer_.sz = 0;
    Request request(MessageType::HardReset, buffer_);
    return serial_.request(request, buffer_).reset_status;
}

ResetStatus Fortuna1RealHardware::soft_reset()
{
    buffer_.sz = 0;
    Request request(MessageType::SoftReset, buffer_);
    return serial_.request(request, buffer_).reset_status;
}

void Fortuna1RealHardware::system_reset()
{
    serial_.send_byte(SYSTEM_RESET);
    std::this_thread::sleep_for(100ms);
}

Z80_Info Fortuna1RealHardware::z80_info() const
{
    buffer_.sz = 0;
    Request request(MessageType::Z80_CpuInfo, buffer_);
    return serial_.request(request, buffer_).z80_info;
}

Z80_Info Fortuna1RealHardware::z80_step()
{
    buffer_.sz = 0;
    Request request(MessageType::Z80_Step, buffer_);
    return serial_.request(request, buffer_).z80_info;
}

Z80_Info Fortuna1RealHardware::z80_nmi()
{
    buffer_.sz = 0;
    Request request(MessageType::Z80_NMI, buffer_);
    return serial_.request(request, buffer_).z80_info;
}

void Fortuna1RealHardware::keypress(uint16_t key)
{
    buffer_.sz = 0;
    Request request(MessageType::Keypress, buffer_);
    request.keypress = key;
    serial_.request(request, buffer_);
}

std::vector<uint16_t> Fortuna1RealHardware::list_breakpoints() const
{
    buffer_.sz = 0;
    Request request(MessageType::BreakpointsList, buffer_);
    serial_.request(request, buffer_);
    return extract_breakpoints_from_buffer(buffer_);
}

std::vector<uint16_t> Fortuna1RealHardware::add_breakpoint(uint16_t address)
{
    buffer_.sz = 0;
    Request request(MessageType::BreakpointsAdd, buffer_);
    request.address = address;
    serial_.request(request, buffer_);
    return extract_breakpoints_from_buffer(buffer_);
}

std::vector<uint16_t> Fortuna1RealHardware::remove_breakpoint(uint16_t address)
{
    buffer_.sz = 0;
    Request request(MessageType::BreakpointsRemove, buffer_);
    request.address = address;
    serial_.request(request, buffer_);
    return extract_breakpoints_from_buffer(buffer_);
}

std::vector<uint16_t> Fortuna1RealHardware::remove_all_breakpoints()
{
    buffer_.sz = 0;
    Request request(MessageType::BreakpointsRemoveAll, buffer_);
    serial_.request(request, buffer_);
    return extract_breakpoints_from_buffer(buffer_);
}

std::vector<uint16_t> Fortuna1RealHardware::extract_breakpoints_from_buffer(Buffer const& buffer)
{
    std::vector<uint16_t> bkps;
    for (size_t i = 0; i < MAX_BREAKPOINTS; ++i) {
        uint16_t bkp = buffer.data[i * 2];
        bkp |= ((uint16_t) buffer.data[i * 2 + 1]) << 8;
        if (bkp != 0)
            bkps.push_back(bkp);
    }
    return bkps;
}

void Fortuna1RealHardware::run()
{

}

void Fortuna1RealHardware::stop()
{

}

EventType Fortuna1RealHardware::last_event() const
{
    return NoEvents;
}

