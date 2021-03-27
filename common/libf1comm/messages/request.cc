#include "request.hh"
#include "../serialization.hh"

void Request::serialize_detail(Message::SerializationFunction f, void* data) const
{
    switch (message_type_) {
        case RamReadByte:
        case RamWriteByte:
        case RamReadBlock:
        case RamWriteBlock:
            ram_request.serialize(f, data);
            break;
        case SDCard_Read:
            serialize_u32(sdcard_block, f, data);
            break;
        case Keypress:
            serialize_u16(keypress, f, data);
            break;
        case BreakpointsAdd:
        case BreakpointsRemove:
            serialize_u16(address, f, data);
            break;
        default:
            break;
    }
}

void Request::deserialize_detail(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2)
{
    switch (message_type_) {
        case RamReadByte:
        case RamWriteByte:
        case RamReadBlock:
        case RamWriteBlock:
            ram_request = RamRequest::unserialize(f, data, sum1, sum2);
            break;
        case SDCard_Read:
            sdcard_block = unserialize_u32(f, data, sum1, sum2);
            break;
        case Keypress:
            keypress = unserialize_u16(f, data, sum1, sum2);
            break;
        case BreakpointsAdd:
        case BreakpointsRemove:
            address = unserialize_u16(f, data, sum1, sum2);
            break;
        default:
            break;
    }
}

#ifndef EMBEDDED
#include <iostream>

bool Request::compare(Message const& message) const
{
    Request& other = *(Request *) &message;
    bool eq = Message::compare(message);
    switch (message_type_) {
        case RamReadByte:
        case RamWriteByte:
        case RamReadBlock:
        case RamWriteBlock:
            if (ram_request != other.ram_request)
                eq = false;
            break;
        case SDCard_Read:
            if (sdcard_block != other.sdcard_block)
                eq = false;
            break;
        case Keypress:
            if (keypress != other.keypress)
                eq = false;
            break;
        case BreakpointsAdd:
        case BreakpointsRemove:
            if (address != other.address)
                eq = false;
            break;
        default:
            break;
    }
    return eq;
}

void Request::debug_detail() const
{
    std::cout << std::hex << std::uppercase;
    switch (message_type_) {
        case RamReadByte:
        case RamWriteByte:
        case RamReadBlock:
        case RamWriteBlock:
            ram_request.debug_detail();
            break;
        case SDCard_Read:
            std::cout << "  sdcard_block: 0x" << sdcard_block << "\n";
            break;
        case Keypress:
            std::cout << "  keypress: 0x" << keypress << "\n";
            break;
        case BreakpointsAdd:
        case BreakpointsRemove:
            std::cout << "  address: 0x" << address << "\n";
            break;
        default:
            break;
    }
}
#endif
