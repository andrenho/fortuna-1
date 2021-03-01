#include "message.hh"

std::string Message::serialize_to_string() const
{
    return "";
}

bool Message::compare(Message const& other) const
{
    return message_type_ == other.message_type_;
}
