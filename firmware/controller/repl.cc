#include "repl.hh"

#include <avr/pgmspace.h>
#include <stdio.h>

// #include <pb_decode.h>
// #include "messages.pb.h"
#include "common/protocol.h"
#include "serial.hh"

#define MAX_MSG_SZ 256

extern Serial serial;

static unsigned int free_ram()
{
    extern int __heap_start, *__brkval;
    volatile int v;
    int free = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    return free;
}

/*
static void repl_do_protobuf()
{
    // get message size
    uint16_t msg_sz = serial.recv16();
    if (msg_sz > MAX_MSG_SZ) {
        serial.send(Z_REQUEST_TOO_LARGE);
    }

    // receive message
    Request request = Request_init_zero;
    pb_istream_t stream = {
        [](pb_istream_t *stream, uint8_t *buf, size_t count) -> bool {
            Serial* serial = static_cast<Serial*>(stream->state);
            for (size_t i = 0; i < count; ++i)
                buf[i] = serial->recv();
            return true;
        },
        &serial,
        MAX_MSG_SZ,
    };
    pb_decode(&stream, Request_fields, &request);

    // calculate checksum
    serial.send(Z_CHECKSUM_NO_MATCH);
}
*/

void repl_do()
{
    uint8_t cmd;
    if (scanf("%c", &cmd) == 0)
        return;
    switch (cmd) {
        case 'f':
            printf_P(PSTR("%d bytes free.\n"), free_ram());
            break;
        /*
        case Z_FOLLOWS_PROTOBUF_REQ:
            repl_do_protobuf();
            break;
        */
    }
}
