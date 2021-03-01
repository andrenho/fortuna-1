#include "reply.hh"
#include "serialization.hh"

void Reply::serialize_detail(Message::SerializationFunction f, void* data) const
{
    serialize_u8(result, f, data);
    switch (message_type_) {
        case FreeMem:
            serialize_u16(free_mem, f, data);
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
        default:
            break;
    }
}

#ifdef TEST
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
        default:
            break;
    }
    return eq;
}
#endif

#ifndef EMBEDDED
#include <iostream>

void Reply::debug_detail() const
{
    switch (message_type_) {
        case FreeMem:
            std::cout << "  free_mem: " << free_mem << "\n";
            break;
        default:
            break;
    }
}
#endif
