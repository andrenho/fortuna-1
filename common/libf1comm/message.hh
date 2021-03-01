#ifndef LIBF1COMM_MESSAGE_HH
#define LIBF1COMM_MESSAGE_HH

#include <cstdint>
#include <string>
#include "message_type.hh"

class Message {
public:
    using SerializationFunction = void(*)(uint8_t byte, void* data);
    using DeserializationFunction = uint8_t(*)(void* data);
    
    virtual ~Message() = default;
    
    void serialize(SerializationFunction f, void* data = nullptr) const;
    
    DeserializationError deserialization_error() const { return deserialization_error_; }

#ifdef TEST
    std::string serialize_to_string() const;
    bool operator==(Message const& rhs) const { return compare(rhs); }
    bool operator!=(Message const& rhs) const { return !(rhs == *this); }
#endif

protected:
             Message()                         : message_type_(MessageType::Undefined) {}
    explicit Message(MessageType message_type) : message_type_(message_type) {}
    
    virtual uint8_t message_class() const = 0;
    virtual void    serialize_detail(SerializationFunction f, void* data) const = 0;

#ifdef TEST
    virtual bool compare(Message const& message) const;
#endif
    
    MessageType          message_type_;
    DeserializationError deserialization_error_ = NoErrors;
    
    static void deserialize_header(Message* message, DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2);
};

template <typename T>
T deserialize_from_string(std::string const& serial)
{
    struct S { std::string const& serial; size_t i; };
    S s { serial, 0 };
    T t = T::deserialize([](void* data) -> uint8_t {
        S* s = (S*) data;
        return s->serial[s->i++];
    }, &s);
    return t;
}

uint8_t add_to_checksum(uint8_t data, uint16_t* sum1, uint16_t* sum2);

#endif
