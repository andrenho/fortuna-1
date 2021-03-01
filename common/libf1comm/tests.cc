#include <cstdio>
#include <cstring>
#include <iostream>

#include "messages/request.hh"
#include "defines.hh"
#include "buffer.hh"
#include "messages/debuginformation.hh"
#include "messages/deserialize.hh"
#include "messages/reply.hh"
#include "fields/ramrequest.hh"

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
        request.debug();
        printf("------------\n");
    }
    {
        DebugInformation debug_information(buffer);
        std::string s = "This is a debug message.";
        strcpy(reinterpret_cast<char*>(buffer.data), s.c_str());
        buffer.sz = s.length();
        assert_serialization("DebugInformation", debug_information);
        debug_information.debug();
        printf("------------\n");
    }
    {
        Reply reply(MessageType::FreeMem);
        reply.free_mem = 1234;
        Reply r = assert_serialization("Reply FreeMem", reply);
        assert_eq("FreeMem conversion", 1234, r.free_mem);
        reply.debug();
        printf("------------\n");
    }
    {
        Request request(MessageType::RamWriteByte);
        RamRequest ram_request = { 0xabcd, 1, 0xfe };
        request.ram_request = ram_request;
        Request r = assert_serialization("RAM request", request);
        assert_eq("Address", ram_request.address, r.ram_request.address);
        assert_eq("Data", ram_request.byte, r.ram_request.byte);
        request.debug();
        printf("------------\n");
    }
}
