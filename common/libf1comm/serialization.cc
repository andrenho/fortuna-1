#include "serialization.hh"

void serialize_u8(uint8_t byte, Message::SerializationFunction f, void* data)
{
    f(byte, data);
}

void serialize_u16(uint16_t word, Message::SerializationFunction f, void* data)
{
    f(word & 0xff, data);
    f(word >> 8, data);
}

void serialize_u32(uint32_t dword, Message::SerializationFunction f, void* data)
{
    f(dword & 0xff, data);
    f((dword >> 8) & 0xff, data);
    f((dword >> 16) & 0xff, data);
    f((dword >> 24) & 0xff, data);
}

uint8_t unserialize_u8(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2)
{
    return add_to_checksum(f(data), sum1, sum2);
}

uint16_t unserialize_u16(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2)
{
    uint16_t value = add_to_checksum(f(data), sum1, sum2);
    value |= (uint16_t) add_to_checksum(f(data), sum1, sum2) << 8;
    return value;
}

uint32_t unserialize_u32(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2)
{
    uint32_t value = add_to_checksum(f(data), sum1, sum2);
    value |= (uint32_t) add_to_checksum(f(data), sum1, sum2) << 8;
    value |= (uint32_t) add_to_checksum(f(data), sum1, sum2) << 16;
    value |= (uint32_t) add_to_checksum(f(data), sum1, sum2) << 24;
    return value;
}
