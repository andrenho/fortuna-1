#include "sdcardstatus.hh"
#include "../serialization.hh"

void SDCardStatus::serialize(Message::SerializationFunction f, void* data) const
{
    serialize_u8(last_stage, f, data);
    serialize_u8(last_response, f, data);
}

SDCardStatus SDCardStatus::unserialize(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2)
{
    SDCardStatus s {};
    s.last_stage = unserialize_u8(f, data, sum1, sum2);
    s.last_response = unserialize_u8(f, data, sum1, sum2);
    return s;
}

#ifndef EMBEDDED
#include <iostream>

void SDCardStatus::debug_detail() const
{
    std::cout << std::hex << std::uppercase;
    std::cout << "  sdcard_status: {\n";
    std::cout << "    last_stage: 0x" << (int) last_stage << "\n";
    std::cout << "    last_response: 0x" << (int) last_response << "\n";
    std::cout << "  }\n";
}

bool SDCardStatus::operator==(SDCardStatus const& rhs) const
{
    return last_stage == rhs.last_stage && last_response == rhs.last_response;
}
#endif
