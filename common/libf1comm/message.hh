#ifndef LIBF1COMM_MESSAGE_HH
#define LIBF1COMM_MESSAGE_HH

#include <stdint.h>
#include "defines.hh"
#include "buffer.hh"

#ifdef TEST
#include <string>
#endif

class Message {
public:
    Message()                                         : message_type_(MessageType::Undefined) {}
    explicit Message(MessageType message_type)        : message_type_(message_type) {}
    explicit Message(Buffer& buffer)                  : message_type_(MessageType::Undefined), buffer_(&buffer) {}
    Message(MessageType message_type, Buffer& buffer) : message_type_(message_type), buffer_(&buffer) {}
    
    using SerializationFunction = void(*)(uint8_t byte, void* data);
    using DeserializationFunction = uint8_t(*)(void* data);
    
    static void deserialize_header(Message* message, DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2, bool skip_first_byte);
    
    virtual ~Message() = default;
    
    void serialize(SerializationFunction f, void* data = nullptr) const;
    virtual void deserialize_detail(DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2) = 0;
    
    DeserializationError deserialization_error() const { return deserialization_error_; }
    void set_deserialization_error(DeserializationError deserialization_error) { deserialization_error_ = deserialization_error; }
    
#ifdef TEST
    std::string serialize_to_string() const;
    bool operator==(Message const& rhs) const { return compare(rhs); }
    bool operator!=(Message const& rhs) const { return !(rhs == *this); }
#endif

#ifndef EMBEDDED
    void debug() const;
protected:
    virtual const char* classname() const = 0;
    virtual void debug_detail() const = 0;
#endif

protected:
    virtual MessageClass message_class() const = 0;
    virtual void         serialize_detail(SerializationFunction f, void* data) const = 0;
    
#ifdef TEST
    virtual bool compare(Message const& message) const;
#endif
    
    MessageType          message_type_;
    DeserializationError deserialization_error_ = DeserializationError::NoErrors;
    Buffer*              buffer_ = nullptr;
};

uint8_t add_to_checksum(uint8_t data, uint16_t* sum1, uint16_t* sum2);

#endif
