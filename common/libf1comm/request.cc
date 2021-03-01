#include "request.hh"

#ifdef TEST
bool Request::compare(Message const& message) const
{
    return Message::compare(message);
}

void Request::serialize_detail(Message::SerializationFunction f, void* data) const
{
}

void Request::deserialize_detail(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2)
{
}

#endif
