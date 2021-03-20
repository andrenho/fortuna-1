#include "reply.hh"
#include "../serialization.hh"

#ifdef EMBEDDED
extern "C" void __cxa_pure_virtual() { while (1); }
#endif

void Reply::serialize_detail(Message::SerializationFunction f, void* data) const
{
    serialize_u8(result, f, data);
    switch (message_type_) {
        case FreeMem:
            serialize_u16(free_mem, f, data);
            break;
        case RamReadByte:
        case RamWriteByte:
            serialize_u8(ram_byte, f, data);
            break;
        case SDCard_Status:
        case SDCard_Read:
            sd_status.serialize(f, data);
            break;
        case SoftReset:
        case HardReset:
            serialize_u8(reset_status, f, data);
            break;
        case Z80_CpuInfo:
        case Z80_Step:
            z80_info.serialize(f, data);
            break;
        default:
            break;
    }
}

void Reply::deserialize_detail(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2)
{
    result = (Result) unserialize_u8(f, data, sum1, sum2);
    switch (message_type_) {
        case FreeMem:
            free_mem = unserialize_u16(f, data, sum1, sum2);
            break;
        case RamReadByte:
        case RamWriteByte:
            ram_byte = unserialize_u8(f, data, sum1, sum2);
            break;
        case SDCard_Status:
        case SDCard_Read:
            sd_status = SDCardStatus::unserialize(f, data, sum1, sum2);
            break;
        case SoftReset:
        case HardReset:
            reset_status = static_cast<ResetStatus>(unserialize_u8(f, data, sum1, sum2));
            break;
        case Z80_CpuInfo:
        case Z80_Step:
            z80_info = Z80_Info::unserialize(f, data, sum1, sum2);
            break;
        default:
            break;
    }
}

#ifndef EMBEDDED
#include <iostream>

bool Reply::compare(Message const& message) const
{
    Reply& other = *(Reply *) &message;
    bool eq = Message::compare(message);
    if (result != other.result)
        eq = false;
    switch (message_type_) {
        case FreeMem:
            if (free_mem != other.free_mem)
                eq = false;
            break;
        case RamReadByte:
        case RamWriteByte:
            if (ram_byte != other.ram_byte)
                eq = false;
            break;
        case SDCard_Status:
        case SDCard_Read:
            if (sd_status != other.sd_status)
                eq = false;
            break;
        case SoftReset:
        case HardReset:
            if (reset_status != other.reset_status)
                eq = false;
            break;
        case Z80_CpuInfo:
        case Z80_Step:
            if (z80_info != other.z80_info)
                eq = false;
        default:
            break;
    }
    return eq;
}

void Reply::debug_detail() const
{
    std::cout << std::hex << std::uppercase;
    switch (message_type_) {
        case FreeMem:
            std::cout << "  free_mem: " << std::dec << free_mem << std::hex << "\n";
            break;
        case RamReadByte:
        case RamWriteByte:
            std::cout << "  ram_byte: 0x" << (int) ram_byte << "\n";
            break;
        case SDCard_Status:
        case SDCard_Read:
            sd_status.debug_detail();
            break;
        case SoftReset:
        case HardReset:
            std::cout << "  reset_status: " << reset_status_names.at(reset_status) << "\n";
            break;
        case Z80_Step:
        case Z80_CpuInfo:
            z80_info.debug_detail();
            break;
        default:
            break;
    }
    std::cout << "  result: 0x" << (int) result << "\n";
}
#endif
