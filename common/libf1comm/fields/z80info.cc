#include "z80info.hh"

#include "../serialization.hh"

void Z80_Info::serialize(Message::SerializationFunction f, void* data) const
{
    serialize_u32(cycle_count, f, data);
    serialize_u16(pc, f, data);
    serialize_u8(last_printed_char, f, data);
}

Z80_Info Z80_Info::unserialize(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2)
{
    Z80_Info z {};
    z.cycle_count = unserialize_u32(f, data, sum1, sum2);
    z.pc = unserialize_u16(f, data, sum1, sum2);
    z.last_printed_char = unserialize_u8(f, data, sum1, sum2);
    return z;
}

#ifndef EMBEDDED
#include <iostream>

void Z80_Info::debug_detail() const
{
    std::cout << std::hex << std::uppercase;
    std::cout << "  z80_info: {\n";
    std::cout << "    pc: " << pc << "\n";
    std::cout << "    last_printed_char: " << (int) last_printed_char;
    if (last_printed_char >= 32 && last_printed_char < 127)
        std::cout << " '" << last_printed_char << "'";
    std::cout << "\n";
    std::cout << "    cycle_count: " << std::dec << cycle_count << "\n";
    std::cout << "  }\n";
}

bool Z80_Info::operator==(Z80_Info const& rhs) const
{
    return cycle_count == rhs.cycle_count
        && pc == rhs.pc
        && last_printed_char == rhs.last_printed_char;
}
#endif