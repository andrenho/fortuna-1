#include "command.hh"

void CommandFreeMem::execute(Reply& reply) {
    extern int __heap_start, *__brkval;
    volatile int v;
    reply.payload.free_mem = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
