#ifndef LIBF1COMM_REQUEST_HH
#define LIBF1COMM_REQUEST_HH

#include "message.hh"

class Request : public Message {
public:
    explicit Request(MessageType message_type) : Message(message_type) {}
    Request(MessageType message_type, Buffer& buffer) : Message(message_type, buffer) {}
    
    static Request deserialize(Buffer& buffer, Message::DeserializationFunction f, void* data);

protected:
    Request() = default;
    explicit Request(Buffer& buffer) : Message(buffer) {}
    
    MessageClass message_class() const override { return MessageClass::MC_Request; }
    void   serialize_detail(Message::SerializationFunction f, void* data) const override;

#ifdef TEST
    bool compare(Message const& message) const override;
#endif
};

#endif
