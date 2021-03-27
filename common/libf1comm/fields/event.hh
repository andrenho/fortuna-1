#ifndef LIBF1COMM_EVENT_HH
#define LIBF1COMM_EVENT_HH

#include <stdint.h>
#include "../message.hh"

struct Event {
    EventType event;
    uint16_t  data;
    
    void serialize(Message::SerializationFunction f, void* data) const;
    static Event unserialize(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2);

#ifndef EMBEDDED
    void debug_detail() const;
    
    bool operator==(Event const& rhs) const;
    bool operator!=(Event const& rhs) const { return !(rhs == *this); }
#endif
};

#endif
