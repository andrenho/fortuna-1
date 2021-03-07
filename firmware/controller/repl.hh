#ifndef REPL_HH_
#define REPL_HH_

#include <libf1comm/messages/reply.hh>
#include <libf1comm/messages/request.hh>
#include "util.hh"
#include "ram.hh"
#include "serial.hh"
#include "sdcard.hh"

class Repl {
public:
    Repl(Serial& serial, RAM& ram, SDCard& sdcard) : serial_(serial), ram_(ram), sdcard_(sdcard) {}

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
    SDCard  sdcard_;
};

#endif
