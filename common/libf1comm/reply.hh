#ifndef LIBF1COMM_REPLY_HH
#define LIBF1COMM_REPLY_HH

#include "message.hh"

class Reply : public Message {
public:
    using Message::Message;
    
    union {
        uint16_t free_mem;
    };
    
    void deserialize_detail(DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2) override;

protected:
    bool compare(Message const& message) const override;
    MessageClass message_class() const override { return MC_Reply; }
    void serialize_detail(SerializationFunction f, void* data) const override;
};

#endif
