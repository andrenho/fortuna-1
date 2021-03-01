#ifndef LIBF1COMM_MESSAGE_HH
#define LIBF1COMM_MESSAGE_HH

#include <cstdint>
#include <string>
#include "message_type.hh"

class Message {
public:
    using SerializationFunction = void(*)(uint8_t byte, void* data);
    virtual ~Message() = default;
    
    void serialize(SerializationFunction f, void* data = nullptr) const;

#ifdef TEST
    std::string serialize_to_string() const;
    bool operator==(Message const& rhs) const { return compare(rhs); }
    bool operator!=(Message const& rhs) const { return !(rhs == *this); }
#endif

protected:
    explicit Message(MessageType message_type) : message_type_(message_type) {}
    
    virtual uint8_t message_class() const = 0;
    virtual void serialize_detail(SerializationFunction f, void* data) const = 0;

#ifdef TEST
    virtual bool compare(Message const& message) const;
#endif
    
    MessageType message_type_;
};

#endif
