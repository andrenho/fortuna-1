#ifndef REPL_HH_
#define REPL_HH_

#include <libf1comm/messages/reply.hh>
#include <libf1comm/messages/request.hh>
#include "util.hh"
#include "devices/ram.hh"
#include "protocol/serial.hh"
#include "devices/sdcard.hh"
#include "fortuna1.hh"
#include "console.hh"

class Repl {
public:
    Repl(Serial& serial, Fortuna1& fortuna1)
        : serial_(serial), fortuna1_(fortuna1), console_(serial_, fortuna1_, buffer_) {}

    void execute();

private:
    void    do_message();
    
    void    send_reply(Reply& reply);
    Request recv_request(bool* status);
    Reply   parse_request(Request const& request);
    void    print_reset_result(ResetResult result);

    Serial&   serial_;
    Fortuna1& fortuna1_;
    Buffer    buffer_ { {0}, 0 };
    Console   console_;
};

#endif
