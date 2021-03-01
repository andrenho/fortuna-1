#include "request.hh"

Request::Request(std::string const& serialized)
{

}

bool Request::compare(Message const& message) const
{
    return Message::compare(message);
}
