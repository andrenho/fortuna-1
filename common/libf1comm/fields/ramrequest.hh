#ifndef LIBF1COMM_RAMREQUEST_HH
#define LIBF1COMM_RAMREQUEST_HH

#include "../message.hh"

struct RamRequest {
    uint16_t address;
    uint16_t size;
    uint8_t  byte;
    
    void serialize(Message::SerializationFunction f, void* data) const;

#ifndef EMBEDDED
    void debug_detail() const;

    bool operator==(RamRequest const& rhs) const;
    bool operator!=(RamRequest const& rhs) const { return !(rhs == *this); }
#endif
    
    static RamRequest unserialize(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2);
};

#endif
