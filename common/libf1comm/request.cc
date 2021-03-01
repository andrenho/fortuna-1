#include "request.hh"

#ifdef TEST
bool Request::compare(Message const& message) const
{
    return Message::compare(message);
}

void Request::serialize_detail(Message::SerializationFunction f, void* data) const
{

}

Request Request::deserialize(Message::DeserializationFunction f, void* data)
{
    Request request;
    Message::deserialize_header(&request, f, data);
    
    f(data);  // TODO - ignore checksum for now
    f(data);
    
    return request;
}

#endif
