#include "request.hh"

void Request::serialize_detail(Message::SerializationFunction f, void* data) const
{
    switch (message_type_) {
        case RamReadByte:
        case RamWriteByte:
        case RamReadBlock:
        case RamWriteBlock:
            ram_request.serialize(f, data);
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
        default:
            break;
    }
}

#ifdef TEST
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
        default:
            break;
    }
    return eq;
}
#endif

#ifndef EMBEDDED
void Request::debug_detail() const
{
    switch (message_type_) {
        case RamReadByte:
        case RamWriteByte:
        case RamReadBlock:
        case RamWriteBlock:
            ram_request.debug_detail();
            break;
        default:
            break;
    }
}
#endif
