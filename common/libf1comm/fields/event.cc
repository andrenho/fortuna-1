#include "event.hh"

#include "../serialization.hh"

void Event::serialize(Message::SerializationFunction f, void* pdata) const
{
    serialize_u8(event, f, pdata);
    serialize_u16(data, f, pdata);
}

Event Event::unserialize(Message::DeserializationFunction f, void* pdata, uint16_t* sum1, uint16_t* sum2)
{
    Event e {};
    e.event = static_cast<EventType>(unserialize_u8(f, pdata, sum1, sum2));
    e.data = unserialize_u16(f, pdata, sum1, sum2);
    return e;
}

#ifndef EMBEDDED
#include <iostream>

void Event::debug_detail() const
{
    std::cout << std::hex << std::uppercase;
    std::cout << "  event: {\n";
    std::cout << "    event_type: " << event_names.at(event) << "\n";
    std::cout << "    data: " << data << "\n";
    std::cout << "  }\n";
}

bool Event::operator==(Event const& rhs) const
{
    return event == rhs.event && data == rhs.data;
}

#endif