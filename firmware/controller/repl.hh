#ifndef REPL_HH_
#define REPL_HH_

#include "messages.pb.h"

void repl_do();
void repl_send_reply(Reply const& reply);

#endif
