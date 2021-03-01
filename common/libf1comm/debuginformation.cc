#include "debuginformation.hh"

DebugInformation DebugInformation::deserialize(Buffer& buffer, Message::DeserializationFunction f, void* data)
{
    uint16_t sum1 = 0, sum2 = 0;
    DebugInformation debug_information(buffer);
    Message::deserialize_header(&debug_information, f, data, &sum1, &sum2);
    
    uint16_t csum1 = f(data);
    uint16_t csum2 = f(data);
    
    if (csum1 != sum1 || csum2 != sum2)
        debug_information.deserialization_error_ = DeserializationError::ChecksumDoesNotMatch;
    
    return debug_information;
}
