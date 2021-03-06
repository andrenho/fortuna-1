#ifndef LIBF1COMM_REPLY_HH
#define LIBF1COMM_REPLY_HH

#include "../message.hh"
#include "../fields/sdcardstatus.hh"
#include "../fields/z80info.hh"
#include "../fields/event.hh"

class Reply : public Message {
public:
    using Message::Message;
    
    Result result = Result::OK;
    union {
        uint16_t     free_mem;
        uint8_t      ram_byte;
        SDCardStatus sd_status;
        Z80_Info     z80_info;
        ResetStatus  reset_status;
        Event        event;
    };
    
    void deserialize_detail(DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2) override;

protected:
    MessageClass message_class() const override { return MC_Reply; }
    void serialize_detail(SerializationFunction f, void* data) const override;

#ifndef EMBEDDED
    bool compare(Message const& message) const override;
    char const* classname() const override { return "Reply"; }
    void debug_detail() const override;
#endif
};

#endif
