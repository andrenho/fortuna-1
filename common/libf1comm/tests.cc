#include <cstdio>
#include <cstring>
#include <iostream>

#include "request.hh"
#include "message_type.hh"
#include "buffer.hh"
#include "debuginformation.hh"
#include "deserialize.hh"
#include "reply.hh"

Buffer buffer {};

template <typename T>
T assert_serialization(std::string const& description, T const& message)
{
    printf("%s... ", description.c_str());
    std::string serialized = message.serialize_to_string();
    for (uint8_t c: serialized)
        printf("\e[0;33m%02X\e[0m ", c);
    printf("\n");
    T from_serialized = deserialize_from_string<T>(buffer, serialized);
    if (from_serialized.deserialization_error() != DeserializationError::NoErrors) {
        printf("\e[0;31mDeseralization error.\e[0m\n");
        exit(EXIT_FAILURE);
    }
    if (message != from_serialized) {
        printf("\e[0;31mReserialized description is different from original.\e[0m\n");
        exit(EXIT_FAILURE);
    }
    return from_serialized;
}

template <typename T, typename U>
void assert_eq(std::string const& description, T&& expected, U&& received)
{
    printf("%s... ", description.c_str());
    if (expected != received) {
        std::cout << "\e[0;31mAssertion error, expected " << expected << ", found " << received << ".\e[0m\n";
        exit(EXIT_FAILURE);
    }
}

int main()
{
    {
        Request request(MessageType::Nop);
        assert_serialization("NOP request", request);
    }
    {
        DebugInformation debug_information(buffer);
        std::string s = "This is a debug message.";
        strcpy(reinterpret_cast<char*>(buffer.data), s.c_str());
        buffer.sz = s.length();
        assert_serialization("DebugInformation", debug_information);
    }
    {
        Reply reply(MessageType::FreeMem);
        reply.free_mem = 1234;
        Reply r = assert_serialization("Reply FreeMem", reply);
        assert_eq("FreeMem conversion", 1234, r.free_mem);
    }
}
