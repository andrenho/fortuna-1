#ifndef LIBF1COMM_REQUEST_HH
#define LIBF1COMM_REQUEST_HH

#include "message.hh"

class Request : public Message {
public:
    explicit Request(MessageType message_type) : Message(message_type) {}
    explicit Request(std::string const& serialized);

protected:
    bool compare(Message const& message) const override;
};

#endif
