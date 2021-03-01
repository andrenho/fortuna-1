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
    uint16_t sum1 = 0, sum2 = 0;
    Request request;
    Message::deserialize_header(&request, f, data, &sum1, &sum2);
    
    uint16_t csum1 = f(data);
    uint16_t csum2 = f(data);
    
    if (csum1 != sum1 || csum2 != sum2)
        request.deserialization_error_ = ChecksumDoesNotMatch;
    
    return request;
}

#endif
