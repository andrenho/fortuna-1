#ifndef LIBF1COMM_DEBUGINFORMATION_HH
#define LIBF1COMM_DEBUGINFORMATION_HH

#include "message.hh"

class DebugInformation : public Message {
public:
    explicit DebugInformation(Buffer& buffer) : Message(buffer) {}
    
    static DebugInformation deserialize(Buffer& buffer, Message::DeserializationFunction f, void* data);
    
protected:
    MessageClass message_class() const override { return MC_DebugInformation; }
    void serialize_detail(SerializationFunction f, void* data) const override {}
};

#endif
