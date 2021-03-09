#ifndef LIBF1COMM_Z80INFO_HH
#define LIBF1COMM_Z80INFO_HH

#include <stdint.h>
#include "../message.hh"

struct Z80_Info {
    uint32_t cycle_count;
    uint16_t pc;
    
    void serialize(Message::SerializationFunction f, void* data) const;
    static Z80_Info unserialize(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2);

#ifndef EMBEDDED
    void debug_detail() const;
    
    bool operator==(Z80_Info const& rhs) const;
    bool operator!=(Z80_Info const& rhs) const { return !(rhs == *this); }
#endif
};

#endif
