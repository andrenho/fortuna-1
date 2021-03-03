#ifndef LIBF1COMM_REQUEST_HH
#define LIBF1COMM_REQUEST_HH

#include "../message.hh"
#include "../fields/ramrequest.hh"

class Request : public Message {
public:
    using Message::Message;
    
    MessageClass message_class() const override { return MessageClass::MC_Request; }
    void   serialize_detail(Message::SerializationFunction f, void* data) const override;
    void   deserialize_detail(DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2) override;
    
    union {
        RamRequest ram_request { 0, 0, 0 };
    };

#ifndef EMBEDDED
    bool compare(Message const& message) const override;

protected:
    char const* classname() const override { return "Request"; }
    void debug_detail() const override;
#endif
};

#endif
