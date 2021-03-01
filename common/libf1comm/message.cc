#include "message.hh"

#ifdef TEST
std::string Message::serialize_to_string() const
{
    std::string s;
    serialize([](uint8_t byte, void* p_s) {
        (*(std::string*) p_s) += byte;
    }, &s);
    return s;
}

bool Message::compare(Message const& other) const
{
    return message_type_ == other.message_type_;
}
#endif

void Message::serialize(Message::SerializationFunction f, void* data) const
{
    uint16_t sum1 = 0, sum2 = 0;
    auto add_byte = [&](uint8_t byte) {
        f(add_to_checksum(byte, &sum1, &sum2), data);
    };
    
    // message class
    add_byte(static_cast<uint8_t>(message_class()));
    
    // message type
    add_byte(static_cast<uint8_t>(message_type_));
    
    // message buffer
    add_byte(0);  // TODO - buffer size
    add_byte(0);
    
    // detail
    struct S {
        SerializationFunction f;
        uint16_t *sum1, *sum2;
        void* data;
    };
    S s { f, &sum1, &sum2, data };
    serialize_detail([](uint8_t byte, void* data) {
        S* s = (S*) data;
        s->f(add_to_checksum(byte, s->sum1, s->sum2), s->data);
    }, &s);
    
    // checksum
    f(sum1, data);
    f(sum2, data);
    
    // final byte
    f(FinalByte, data);
}

void Message::deserialize_header(Message* message, Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2)
{
    if (add_to_checksum(f(data), sum1, sum2) != message->message_class()) {
        message->deserialization_error_ = DeserializationError::InvalidMessageClass;
    }
    message->message_type_ = static_cast<MessageType>(add_to_checksum(f(data), sum1, sum2));
    add_to_checksum(f(data), sum1, sum2);  // TODO - ignore buffer size for now
    add_to_checksum(f(data), sum1, sum2);
}

uint8_t add_to_checksum(uint8_t data, uint16_t* sum1, uint16_t* sum2)
{
    *sum1 = (*sum1 + data) % 0xff;
    *sum2 = (*sum2 + *sum1) % 0xff;
    return data;
}