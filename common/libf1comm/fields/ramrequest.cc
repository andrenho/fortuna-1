#include "ramrequest.hh"

#include "../serialization.hh"

void RamRequest::serialize(Message::SerializationFunction f, void* data) const
{
    serialize_u16(address, f, data);
    serialize_u16(size, f, data);
    serialize_u8(byte, f, data);
}

RamRequest RamRequest::unserialize(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2)
{
    RamRequest r {};
    r.address = unserialize_u16(f, data, sum1, sum2);
    r.size = unserialize_u16(f, data, sum1, sum2);
    r.byte = unserialize_u8(f, data, sum1, sum2);
    return r;
}

#ifndef EMBEDDED
#include <iostream>

void RamRequest::debug_detail() const
{
    std::cout << std::hex << std::uppercase;
    std::cout << "  ram_request: {\n";
    std::cout << "    address: 0x" << address << "\n";
    std::cout << "    size: 0x" << size << "\n";
    std::cout << "    byte: 0x" << (int) byte << "\n";
    std::cout << "  }\n";
}

bool RamRequest::operator==(RamRequest const& rhs) const
{
    return address == rhs.address &&
           size == rhs.size &&
           byte == rhs.byte;
}
#endif