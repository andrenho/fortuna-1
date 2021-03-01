#include "reply.hh"
#include "serialization.hh"

void Reply::serialize_detail(Message::SerializationFunction f, void* data) const
{
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
    switch (message_type_) {
        case FreeMem:
            free_mem = unserialize_u16(f, data, sum1, sum2);
            break;
        default:
            break;
    }
}

bool Reply::compare(Message const& message) const
{
    Reply& other = *(Reply *) &message;
    bool eq = Message::compare(message);
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
