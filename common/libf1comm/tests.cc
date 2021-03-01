#include <cstdio>

#include "request.hh"
#include "message_type.hh"

template <typename T>
void assert_serialization(std::string const& description, T const& message)
{
    printf("%s...", description.c_str());
    std::string serialized = message.serialize_to_string();
    for (uint8_t c: serialized)
        printf("\e[0;33m%02X\e[0m ", c);
    printf("\n");
    T from_serialized(serialized);
    if (message != from_serialized) {
        printf("\e[0;31mReserialized description is different from original.\e[0m\n");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    {
        Request request(MessageType::Nop);
        assert_serialization("NOP request", request);
    }
}
