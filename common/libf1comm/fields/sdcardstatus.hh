#ifndef LIBF1COMM_SDCARDSTATUS_HH
#define LIBF1COMM_SDCARDSTATUS_HH

#include "../message.hh"

struct SDCardStatus {
    uint8_t last_stage;
    uint8_t last_response;
    
    void serialize(Message::SerializationFunction f, void* data) const;
    static SDCardStatus unserialize(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2);

#ifndef EMBEDDED
    void debug_detail() const;
    
    bool operator==(SDCardStatus const& rhs) const;
    bool operator!=(SDCardStatus const& rhs) const { return !(rhs == *this); }
#endif
};

#endif
