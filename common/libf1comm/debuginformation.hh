#ifndef LIBF1COMM_DEBUGINFORMATION_HH
#define LIBF1COMM_DEBUGINFORMATION_HH

#include "message.hh"

class DebugInformation : public Message {
public:
    using Message::Message;
    
protected:
    MessageClass message_class() const override { return MC_DebugInformation; }
    void serialize_detail(SerializationFunction f, void* data) const override {}
};

#endif
