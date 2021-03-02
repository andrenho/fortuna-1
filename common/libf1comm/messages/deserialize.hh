#ifndef LIBF1COMM_DESERIALIZE_HH
#define LIBF1COMM_DESERIALIZE_HH

template <typename T>
T deserialize(Buffer& buffer, Message::DeserializationFunction f, void* data, bool skip_first_byte)
{
    uint16_t sum1 = 0, sum2 = 0;
    T message(buffer);
    Message::deserialize_header(&message, f, data, &sum1, &sum2, skip_first_byte);
    
    message.deserialize_detail(f, data, &sum1, &sum2);
    
    uint16_t csum1 = f(data);
    uint16_t csum2 = f(data);
    
    if (csum1 != sum1 || csum2 != sum2)
        message.set_deserialization_error(DeserializationError::ChecksumDoesNotMatch);
    
    if (f(data) != FinalByte)
        message.set_deserialization_error(DeserializationError::FinalByteNotReceived);
    
    return message;
}

#ifndef EMBEDDED
template <typename T>
T deserialize_from_string(Buffer& buffer, std::string const& serial)
{
    struct S { std::string const& serial; size_t i; };
    S s { serial, 0 };
    T t = deserialize<T>(buffer, [](void* data) -> uint8_t {
        S* s = (S*) data;
        return s->serial[s->i++];
    }, &s, false);
    return t;
}
#endif


#endif //LIBF1COMM_DESERIALIZE_HH
