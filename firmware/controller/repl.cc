#include "repl.hh"

#include <avr/pgmspace.h>
#include <stdio.h>

#include <pb_decode.h>
#include <pb_encode.h>
#include "common/protocol.h"

#define MAX_MSG_SZ 1024

// for every new command, include:
//   - Command
//   - Repl::do_terminal
//   - Repl::parse_request

void Repl::do_terminal(char cmd)
{
    switch (cmd) {
        case 'f':
            printf_P(PSTR("%d bytes free.\n"), command_.free_ram());
            break;
        case 'D':
            printf_P(PSTR("- %s\n"), command_.test_dma());
            break;
        default:
            printf_P(PSTR("Syntax error.\n"));
    }
}

Reply Repl::parse_request(Request const& request)
{
    Reply reply;
    reply.type = request.type;
    reply.result = Result_OK;
    reply.which_payload = 0;
    switch (request.type) {
        case MessageType_FREE_MEM:
            reply.which_payload = Reply_freeMem_tag;
            reply.payload.freeMem.amount = command_.free_ram();
            break;
        case MessageType_TEST_DEBUG:
            command_.test_debug_messages();
            break;
        case MessageType_TEST_DMA:
            reply.which_payload = Reply_testDMA_tag;
            reply.payload.testDMA.response.arg = (void*) command_.test_dma();
            reply.payload.testDMA.response.funcs.encode = [](pb_ostream_t* stream, const pb_field_t* field, void* const* arg) {
                const char* buf = (const char*) (*arg);
                if (!pb_encode_tag_for_field(stream, field))
                    return false;
                return pb_encode_string(stream, (uint8_t*) buf, strlen(buf));
            };
            break;
        default:
            reply.result = Result_INVALID_REQUEST;
    }
    return reply;
}

Request Repl::recv_request(bool* status)
{
    Request request = Request_init_zero;

    // get message size
    uint16_t msg_sz = serial_.recv16();
    if (msg_sz > MAX_MSG_SZ) {
        serial_.send(Z_REQUEST_TOO_LARGE);
        *status = false;
        return request;
    }

    serial_.reset_checksum();

    // receive message
    pb_istream_t istream = {
        [](pb_istream_t* stream, uint8_t *buf, size_t count) -> bool {
            Serial* serial_ = static_cast<Serial*>(stream->state);
            for (size_t i = 0; i < count; ++i) {
                buf[i] = serial_->recv();
                serial_->add_to_checksum(buf[i]);
            }
            return true;
        },
        &serial_,
        msg_sz,
        nullptr
    };
    if (!pb_decode(&istream, Request_fields, &request)) {
        serial_.send(Z_ERROR_DECODING_REQUEST);
        *status = false;
        return request;
    }

    // calculate checksum
    uint16_t sum2 = serial_.recv();
    uint16_t sum1 = serial_.recv();
    if (!serial_.compare_checksum(sum1, sum2)) {
        serial_.send(Z_CHECKSUM_NO_MATCH);
        *status = false;
        return request;
    }

    // get request over
    if (serial_.recv() != Z_REQUEST_OVER) {
        serial_.send(Z_REQUEST_NOT_OVER);
        *status = false;
    }

    return request;
}

size_t Repl::message_size(Reply const& reply)
{
    pb_ostream_t szstream = {0, nullptr, 0, 0, nullptr};
    if (!pb_encode(&szstream, Reply_fields, &reply))
        return 0xffff;
    return szstream.bytes_written;
}

void Repl::send_reply(Reply const& reply)
{
    size_t sz = message_size(reply);
    if (sz > MAX_MSG_SZ) {
        serial_.send(Z_RESPONSE_TOO_LARGE);
        return;
    }

    serial_.send(Z_FOLLOWS_PROTOBUF_RESP);
    serial_.send((sz >> 8) & 0xff);
    serial_.send(sz & 0xff);

    serial_.reset_checksum();

    pb_ostream_t ostream = {
        [](pb_ostream_t* stream, const uint8_t* buf, size_t count) -> bool {
            Serial* serial_ = static_cast<Serial*>(stream->state);
            for (size_t i = 0; i < count; ++i) {
                serial_->send(buf[i]);
                serial_->add_to_checksum(buf[i]);
            }
            return true;
        },
        &serial_,
        MAX_MSG_SZ,
        0,
        nullptr
    };
    if (!pb_encode(&ostream, Reply_fields, &reply)) {
        serial_.send(Z_ERROR_ENCODING_REPLY);
        return;
    }

    // send checksum
    auto chk = serial_.checksum();
    serial_.send(chk.sum2);
    serial_.send(chk.sum1);

    // send reply over
    serial_.send(Z_REPLY_OVER);
}

void Repl::do_protobuf()
{
    bool status = true;
    Request request = recv_request(&status);
    if (!status)
        return;

    Reply reply = parse_request(request);
    send_reply(reply);
}

void Repl::execute()
{
    uint8_t cmd = getchar();
    if (cmd == Z_FOLLOWS_PROTOBUF_REQ)
        do_protobuf();
    else if (cmd >= ' ' && cmd <= '~')
        do_terminal(cmd);
    else
        serial_.send(Z_INVALID_COMMAND);
}
