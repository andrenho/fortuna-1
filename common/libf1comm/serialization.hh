#ifndef LIBF1COMM_SERIALIZATION_HH
#define LIBF1COMM_SERIALIZATION_HH

#include <stdint.h>
#include "message.hh"

void serialize_u8(uint8_t byte, Message::SerializationFunction f, void* data);
void serialize_u16(uint16_t word, Message::SerializationFunction f, void* data);
void serialize_u32(uint32_t dword, Message::SerializationFunction f, void* data);

uint8_t unserialize_u8(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2);
uint16_t unserialize_u16(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2);
uint32_t unserialize_u32(Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2);

#endif //LIBF1COMM_SERIALIZATION_HH
