#ifndef REPL_HH_
#define REPL_HH_

#include "command.hh"
#include "messages.pb.h"
#include "ram.hh"
#include "serial.hh"

class Repl {
public:
    Repl(Serial& serial, RAM& ram) : serial_(serial), command_(serial, ram), ram_(ram) {}

    void execute();

private:
    void    do_protobuf();
    void send_reply(Reply const& reply, uint8_t string[512]);
    static size_t  message_size(Reply const& reply);
    Request recv_request(bool* status, uint8_t string[512]);
    Reply parse_request(Request const& request, uint8_t string[512]);
    void    do_terminal(char cmd);

    Serial& serial_;
    Command command_;
    RAM     ram_;
};

#endif
