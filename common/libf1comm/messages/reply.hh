#ifndef LIBF1COMM_REPLY_HH
#define LIBF1COMM_REPLY_HH

#include "../message.hh"

class Reply : public Message {
public:
    using Message::Message;
    
    Result result = Result::OK;
    union {
        uint16_t free_mem;
        uint8_t  ram_byte;
    };
    
    void deserialize_detail(DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2) override;

protected:
#if TEST
    bool compare(Message const& message) const override;
#endif
    
    MessageClass message_class() const override { return MC_Reply; }
    void serialize_detail(SerializationFunction f, void* data) const override;
    
#ifndef EMBEDDED
    char const* classname() const override { return "Request"; }
    void debug_detail() const override;
#endif
};

#endif
