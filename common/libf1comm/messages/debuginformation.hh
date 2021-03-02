#ifndef LIBF1COMM_DEBUGINFORMATION_HH
#define LIBF1COMM_DEBUGINFORMATION_HH

#include "../message.hh"

class DebugInformation : public Message {
public:
    using Message::Message;
    
    void deserialize_detail(DeserializationFunction, void*, uint16_t*, uint16_t*) override { }
    
protected:
    MessageClass message_class() const override { return MC_DebugInformation; }
    void serialize_detail(SerializationFunction, void*) const override {}
#ifndef EMBEDDED
    char const* classname() const override { return "DebugInformation"; }
    void debug_detail() const override { }
#endif
};

#endif
