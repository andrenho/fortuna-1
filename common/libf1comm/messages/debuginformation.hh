#ifndef LIBF1COMM_DEBUGINFORMATION_HH
#define LIBF1COMM_DEBUGINFORMATION_HH

#include "../message.hh"

class DebugInformation : public Message {
public:
    using Message::Message;
    
    void deserialize_detail(DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2) override { }
    
protected:
    MessageClass message_class() const override { return MC_DebugInformation; }
    void serialize_detail(SerializationFunction f, void* data) const override {}
#ifndef EMBEDDED
    char const* classname() const override { return "DebugInformation"; }
    void debug_detail() const override { }
#endif
};

#endif
