#include "repl.hh"

#include <avr/pgmspace.h>
#include <stdio.h>

#include <pb_decode.h>
#include <pb_encode.h>
#include "messages.pb.h"
#include "common/protocol.h"
#include "serial.hh"

#define MAX_MSG_SZ 768

extern Serial serial;

static unsigned int free_ram()
{
    extern int __heap_start, *__brkval;
    volatile int v;
    int free = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    return free;
}

static Request repl_recv_request(bool* status)
{
    Request request = Request_init_zero;

    // get message size
    uint16_t msg_sz = serial.recv16();
    if (msg_sz > MAX_MSG_SZ) {
        serial.send(Z_REQUEST_TOO_LARGE);
        *status = false;
        return request;
    }

    serial.reset_checksum();

    // receive message
    pb_istream_t istream = {
        [](pb_istream_t* stream, uint8_t *buf, size_t count) -> bool {
            Serial* serial = static_cast<Serial*>(stream->state);
            for (size_t i = 0; i < count; ++i) {
                buf[i] = serial->recv();
                serial->add_to_checksum(buf[i]);
            }
            return true;
        },
        &serial,
        msg_sz,
        nullptr
    };
    if (!pb_decode(&istream, Request_fields, &request)) {
        serial.send(Z_ERROR_DECODING_REQUEST);
        *status = false;
        return request;
    }

    // calculate checksum (TODO)
    uint16_t sum2 = serial.recv();
    uint16_t sum1 = serial.recv();
    if (!serial.compare_checksum(sum1, sum2)) {
        serial.send(Z_CHECKSUM_NO_MATCH);
        *status = false;
        return request;
    }

    // get request over
    if (serial.recv() != Z_REQUEST_OVER) {
        serial.send(Z_REQUEST_NOT_OVER);
        *status = false;
    }

    return request;
}

static Reply parse_repl_request(Request const& request)
{
    Reply reply;
    reply.type = MessageType_FREE_MEM;
    reply.result = Result_OK;
    reply.which_payload = Reply_free_mem_tag;
    reply.payload.free_mem = 1234;
    return reply;
}

static size_t repl_size(Reply const& reply)
{
    pb_ostream_t szstream = {0};
    pb_encode(&szstream, Reply_fields, &reply);
    return szstream.bytes_written;
}

static void repl_send_reply(Reply const& reply)
{
    size_t sz = repl_size(reply);
    if (sz > MAX_MSG_SZ) {
        serial.send(Z_RESPONSE_TOO_LARGE);
        return;
    }

    serial.send(Z_FOLLOWS_PROTOBUF_RESP);
    serial.send((sz >> 8) & 0xff);
    serial.send(sz & 0xff);

    serial.reset_checksum();

    pb_ostream_t ostream = {
        [](pb_ostream_t* stream, const uint8_t* buf, size_t count) -> bool {
            Serial* serial = static_cast<Serial*>(stream->state);
            for (size_t i = 0; i < count; ++i) {
                serial->send(buf[i]);
                serial->add_to_checksum(buf[i]);
            }
            return true;
        },
        &serial,
        MAX_MSG_SZ,
        0,
        nullptr
    };
    if (!pb_encode(&ostream, Reply_fields, &reply)) {
        serial.send(Z_ERROR_ENCODING_REPLY);
        return;
    }

    // send checksum
    auto chk = serial.checksum();
    serial.send(chk.sum2);
    serial.send(chk.sum1);

    // send reply over
    serial.send(Z_REPLY_OVER);
}

static void repl_do_protobuf()
{
    bool status = true;
    Request request = repl_recv_request(&status);
    if (!status)
        return;

    Reply reply = parse_repl_request(request);
    repl_send_reply(reply);
}

void repl_do()
{
    uint8_t cmd = getchar();
    switch (cmd) {
        case 'f':
            printf_P(PSTR("%d bytes free.\n"), free_ram());
            break;
        case Z_FOLLOWS_PROTOBUF_REQ:
            repl_do_protobuf();
            break;
    }
}
