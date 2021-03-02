#ifndef SERIAL_HH_
#define SERIAL_HH_

#include <string>
#include "libf1comm/messages/reply.hh"
#include "libf1comm/messages/request.hh"

class Serial {
public:
    explicit Serial(const char* port);
    ~Serial();

    Reply request(Request const& request, Buffer& buffer) const;

    void set_log_bytes(bool v) { log_bytes_ = v; }
    void set_log_message(bool v) { log_message_ = v; }

private:
    int  fd = -1;
    bool log_bytes_ = false;
    bool log_message_ = false;
    
    void send_request(Request const& request) const;
    Reply receive_reply(Buffer& buffer) const;
};

#endif
