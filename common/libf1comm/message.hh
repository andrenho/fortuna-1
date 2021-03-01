#ifndef LIBF1COMM_MESSAGE_HH
#define LIBF1COMM_MESSAGE_HH

#include <string>
#include "message_type.hh"

class Message {
public:
    virtual ~Message() = default;

#ifdef TEST
    std::string serialize_to_string() const;
#endif
    
    bool operator==(Message const& rhs) const { return compare(rhs); }
    bool operator!=(Message const& rhs) const { return !(rhs == *this); }

protected:
    explicit Message(MessageType message_type) : message_type_(message_type) {}
    Message() = default;
    
    virtual bool compare(Message const& message) const;
    
    MessageType message_type_;
};

#endif
