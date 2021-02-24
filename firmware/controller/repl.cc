#include "repl.hh"

#include <avr/pgmspace.h>
#include <stdio.h>

#include <pb_decode.h>
#include <pb_encode.h>
#include "messages.pb.h"
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

static Reply parse_response(Request* request)
{
    Reply reply;
    reply.type = MessageType_FREE_MEM;
    reply.result = Result_OK;
    reply.payload.free_mem = 1234;
    return reply;
}

static void repl_do_protobuf()
{
    // get message size
    uint16_t msg_sz = serial.recv16();
    if (msg_sz > MAX_MSG_SZ) {
        serial.send(Z_REQUEST_TOO_LARGE);
        return;
    }

    struct Data {
        Serial* serial;
        int     bytes;
    };

    // receive message
    Request request = Request_init_zero;
    Data data = { &serial, msg_sz };
    pb_istream_t istream = {
        [](pb_istream_t* stream, uint8_t *buf, size_t count) -> bool {
            Data* data = static_cast<Data*>(stream->state);
            for (size_t i = 0; i < count; ++i) {
                buf[i] = data->serial->recv();
                --data->bytes;
                if (data->bytes == 0) {
                    return false;
                }
            }
            return true;
        },
        &serial,
        MAX_MSG_SZ,
        nullptr
    };
    pb_decode(&istream, Request_fields, &request);

    // calculate checksum (TODO)
    serial.recv();
    serial.recv();
    
    // reply
    Reply reply = parse_response(&request);
    serial.send(Z_FOLLOWS_PROTOBUF_RESP);
    pb_ostream_t ostream = {
        [](pb_ostream_t* stream, const uint8_t* buf, size_t count) -> bool {
            Serial* serial = static_cast<Serial*>(stream->state);
            for (size_t i = 0; i < count; ++i)
                serial->send(buf[i]);
            return true;
        },
        &serial,
        MAX_MSG_SZ,
        0,
        nullptr
    };
    pb_encode(&ostream, Reply_fields, &reply);

    // send checksum (TODO)
    serial.send(0xff);
    serial.send(0xff);
}

void repl_do()
{
    uint8_t cmd;
    if (scanf("%c", &cmd) == 0)
        return;
    switch (cmd) {
        case 'f':
            printf_P(PSTR("%d bytes free.\n"), free_ram());
            break;
        case Z_FOLLOWS_PROTOBUF_REQ:
            repl_do_protobuf();
            break;
    }
}
