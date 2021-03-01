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
        f(byte, data);
        sum1 = (sum1 + byte) % 0xff;
        sum2 = (sum2 + sum1) % 0xff;
    };
    
    add_byte(message_class());
    
    struct S {
        SerializationFunction f;
        uint16_t *sum1, *sum2;
        void* data;
    };
    S s { f, &sum1, &sum2, data };
    serialize_detail([](uint8_t byte, void* data) {
        S* s = (S*) data;
        s->f(byte, s->data);
        *s->sum1 = (*s->sum1 + byte) % 0xff;
        *s->sum2 = (*s->sum2 + *s->sum1) % 0xff;
    }, &s);
    
    add_byte(0);  // TODO - buffer size
    add_byte(0);
    
    f(sum2, data);
    f(sum1, data);
}
