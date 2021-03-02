#ifndef REPL_HH_
#define REPL_HH_

#include <libf1comm/messages/reply.hh>
#include <libf1comm/messages/request.hh>
#include "util.hh"
#include "ram.hh"
#include "serial.hh"

class Repl {
public:
    Repl(Serial& serial, RAM& ram) : serial_(serial), ram_(ram) {}

    void execute();

private:
    void    do_message();
    void    do_terminal(char cmd);
    
    void    send_reply(Reply& reply);
    Request recv_request(bool* status);
    Reply   parse_request(Request const& request);

    Serial& serial_;
    RAM     ram_;
    Buffer  buffer_ { {0}, 0 };
};

#endif
