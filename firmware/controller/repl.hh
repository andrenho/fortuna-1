#ifndef REPL_HH_
#define REPL_HH_

#include <libf1comm/messages/reply.hh>
#include <libf1comm/messages/request.hh>
#include "command.hh"
#include "ram.hh"
#include "serial.hh"

class Repl {
public:
    Repl(Serial& serial, RAM& ram) : serial_(serial), command_(serial, ram), ram_(ram) {}

    void execute();

private:
    struct Buffer {
        size_t  size;
        uint8_t buffer[512];
    };
    
    void    do_message();
    void    do_terminal(char cmd);
    
    void    send_reply(Reply& reply, Buffer& buffer);
    Request recv_request(bool* status, Buffer& buffer);
    Reply   parse_request(Request const& request, Buffer& buffer);

    Serial& serial_;
    Command command_;
    RAM     ram_;
};

#endif
