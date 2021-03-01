#include "serialization.hh"

void serialize_u16(uint16_t word, Message::SerializationFunction f, void* data)
{
    f(word & 0xff, data);
    f(word >> 8, data);
}

uint16_t unserialize_u16(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2)
{
    uint16_t value = add_to_checksum(f(data), sum1, sum2);
    value |= (uint16_t) add_to_checksum(f(data), sum1, sum2) << 8;
    return value;
}
