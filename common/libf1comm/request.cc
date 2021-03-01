#include "request.hh"

#ifdef TEST
bool Request::compare(Message const& message) const
{
    return Message::compare(message);
}

void Request::serialize_detail(Message::SerializationFunction f, void* data) const
{
}

#endif
