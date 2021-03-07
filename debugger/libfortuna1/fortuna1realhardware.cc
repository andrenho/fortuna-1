#include "fortuna1realhardware.hh"
#include "replyexception.hh"
#include "libf1comm/messages/request.hh"

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

uint8_t Fortuna1RealHardware::data_bus() const
{
    buffer_.sz = 0;
    Request request(MessageType::DataReadBus, buffer_);
    return serial_.request(request, buffer_).ram_byte;
}

void Fortuna1RealHardware::set_data_bus(uint8_t data)
{
    buffer_.sz = 0;
    Request request(MessageType::DataWriteBus, buffer_);
    request.ram_request.byte = data;
    serial_.request(request, buffer_);
}
