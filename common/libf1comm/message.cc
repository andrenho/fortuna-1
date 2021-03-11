#include "message.hh"

uint8_t add_to_checksum(uint8_t data, uint16_t* sum1, uint16_t* sum2)
{
    *sum1 = (*sum1 + data) % 0xff;
    *sum2 = (*sum2 + *sum1) % 0xff;
    return data;
}

void Message::serialize(Message::SerializationFunction f, void* data) const
{
    uint16_t sum1 = 0, sum2 = 0;
    auto add_byte = [&](uint8_t byte) {
        f(add_to_checksum(byte, &sum1, &sum2), data);
    };
    
    // message class
    f(static_cast<uint8_t>(message_class()), data);
    
    // message type
    add_byte(static_cast<uint8_t>(message_type_));
    
    // message buffer
    if (buffer_ == nullptr || buffer_->sz == 0) {
        add_byte(0);
        add_byte(0);
    } else {
        add_byte(buffer_->sz & 0xff);
        add_byte((buffer_->sz >> 8) & 0xff);
        for (uint16_t i = 0; i < buffer_->sz; ++i)
            add_byte(buffer_->data[i]);
    }
    
    // detail
    struct S {
        SerializationFunction f;
        uint16_t *sum1, *sum2;
        void* data;
    };
    S s { f, &sum1, &sum2, data };
    serialize_detail([](uint8_t byte, void* data) {
        S* s = (S*) data;
        s->f(add_to_checksum(byte, s->sum1, s->sum2), s->data);
    }, &s);
    
    // checksum
    f(sum1, data);
    f(sum2, data);
    
    // final byte
    f(FinalByte, data);
}

void Message::deserialize_header(Message* message, Message::DeserializationFunction f, void* data, uint16_t* sum1, uint16_t* sum2, bool skip_first_byte)
{
    if (!skip_first_byte) {
        if (f(data) != message->message_class()) {
            message->deserialization_error_ = DeserializationError::InvalidMessageClass;
        }
    }
    message->message_type_ = static_cast<MessageType>(add_to_checksum(f(data), sum1, sum2));
    
    message->buffer_->sz = 0;
    message->buffer_->sz |= add_to_checksum(f(data), sum1, sum2);
    message->buffer_->sz |= ((uint16_t) add_to_checksum(f(data), sum1, sum2) << 8);
    if (message->buffer_->sz > 512)
        message->deserialization_error_ = DeserializationError::BufferDataTooLarge;
    else
        for (uint16_t i = 0; i < message->buffer_->sz; ++i)
            message->buffer_->data[i] = add_to_checksum(f(data), sum1, sum2);
}

#ifndef EMBEDDED
#include <iostream>
#include <cstring>

std::string Message::serialize_to_string() const
{
    std::string s;
    serialize([](uint8_t byte, void* p_s) {
        (*(std::string*) p_s) += byte;
    }, &s);
    return s;
}

bool Message::compare(Message const& other) const
{
    bool eq = true;
    if (message_type_ != other.message_type_)
        eq = false;
    if (buffer_ != nullptr && buffer_->sz > 0 && other.buffer_ == nullptr)
        eq = false;
    if (buffer_ == nullptr && other.buffer_ != nullptr && other.buffer_->sz > 0)
        eq = false;
    if (buffer_ != nullptr && other.buffer_ != nullptr) {
        if (buffer_->sz != other.buffer_->sz)
            eq = false;
        if (memcmp(buffer_->data, other.buffer_->data, buffer_->sz) != 0)
            eq = false;
    }
    return eq;
}

void Message::debug() const
{
    std::cout << std::hex << std::uppercase;
    std::cout << classname() << " {\n";
    try {
        std::cout << "  message_type: " << message_type_names.at(message_type_) << " (0x" << (int) message_type_<< ")\n";
    } catch (std::out_of_range&) {
        std::cout << "  message_type: unknown (0x" << (int) message_type_ << ")\n";
    }
    if (buffer_) {
        std::cout << "  buffer: ";
        /*
        if (buffer_is_string())
            std::cout << '"' << (const char*) buffer_ << "\" ";
        */
        for (size_t i = 0; i < buffer_->sz; ++i)
            printf("%02X ", buffer_->data[i]);
        std::cout << "\n";
    }
    debug_detail();
    std::cout << "}\n";
}
#endif
