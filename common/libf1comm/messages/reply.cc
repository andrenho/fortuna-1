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
        case DataReadBus:
            serialize_u8(ram_byte, f, data);
            break;
        case SDCard_Status:
        case SDCard_Read:
            sd_status.serialize(f, data);
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
        case DataReadBus:
            ram_byte = unserialize_u8(f, data, sum1, sum2);
            break;
        case SDCard_Status:
        case SDCard_Read:
            sd_status = SDCardStatus::unserialize(f, data, sum1, sum2);
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
        case DataReadBus:
            if (ram_byte != other.ram_byte)
                eq = false;
            break;
        case SDCard_Status:
        case SDCard_Read:
            if (sd_status != other.sd_status)
                eq = false;
            break;
        default:
            break;
    }
    return eq;
}

void Reply::debug_detail() const
{
    switch (message_type_) {
        case FreeMem:
            std::cout << "  free_mem: " << free_mem << "\n";
            break;
        case RamReadByte:
        case RamWriteByte:
        case DataReadBus:
            std::cout << "  ram_byte: " << (int) ram_byte << "\n";
            break;
        case SDCard_Status:
        case SDCard_Read:
            sd_status.debug_detail();
            break;
        default:
            break;
    }
    std::cout << "  result: " << (int) result << "\n";
}
#endif
