#include "repl.hh"

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <avr/pgmspace.h>

#include <libf1comm/defines.hh>
#include <libf1comm/messages/request.hh>
#include <libf1comm/messages/reply.hh>
#include <libf1comm/messages/deserialize.hh>

// for every new command, include:
//   - Repl::do_terminal
//   - Repl::parse_request

void Repl::do_terminal(char cmd)
{
    const int ERROR = INT_MIN;
    
    auto error = [](){ printf_P(PSTR("Syntax error.\n")); };
    
    auto ask_value_P = [&](const char* question) -> int {
        int value = 0;
        printf_P(question);
        putchar('?');
        putchar(' ');
        serial_.set_echo(true);
        int n = scanf("%i", &value);
        serial_.set_echo(true);
        if (n != 1) {
            error();
            return ERROR;
        } else {
            putchar('\n');
            return value;
        }
    };
    
    struct Values { int v1, v2; };
    auto ask_two_values_P = [&](const char* question) -> Values {
        Values values = { 0, 0 };
        printf_P(question);
        putchar('?');
        putchar(' ');
        serial_.set_echo(true);
        int n = scanf("%i %i", &values.v1, &values.v2);
        serial_.set_echo(true);
        if (n != 2) {
            error();
            return { ERROR, 0 };
        } else {
            putchar('\n');
            return values;
        }
    };
    
    putchar(cmd);
    putchar('\n');
    switch (cmd) {
        case 'h':
        case '?':
            printf_P(PSTR("[f] bytes free  [D] test DMA\n"));
            printf_P(PSTR("[r] read byte  [w] write byte  [W] write multiple bytes  [d] dump memory\n"));
            break;
        case 'f':
            printf_P(PSTR("%d bytes free.\n"), free_ram());
            break;
        case 'D':
            printf_P(PSTR("- %s\n"), ram_.test());
            break;
        case 'r': {
                int addr = ask_value_P(PSTR("Addr"));
                if (addr != ERROR)
                    printf_P(PSTR("0x%02X\n"), ram_.read_byte(addr));
            }
            break;
        case 'w': {
                Values vv = ask_two_values_P(PSTR("Addr Data"));
                if (vv.v1 != ERROR) {
                    printf_P(PSTR("0x%02X\n"), ram_.write_byte(vv.v1, vv.v2));
                }
            }
            break;
        case 'd': {
                int page = ask_value_P(PSTR("Page (0x100)"));
                if (page != ERROR) {
                    // if (!ram_.write_block(page * 0x100, 0x100, [](uint16_t idx)
                    if (!ram_.read_block(page * 0x100, 0x100, [](uint16_t addr, uint8_t byte, void* page) {
                        if (addr % 0x10 == 0)
                            printf_P(PSTR("%04X : "), addr + (*(int*)page * 0x100));
                        printf_P(PSTR("%02X "), byte);
                        if (addr % 0x10 == 0xf)
                            putchar('\n');
                    }, &page))
                        printf_P(PSTR("Checksum error\n"));
                }
            }
            break;
        /*
        case 'W': {
                int addr = ask_value_P(PSTR("Addr"));
                if (addr != ERROR)
                    break;
                int nbytes = ask_value_P(PSTR("# bytes"));
                for (int i = 0; i < nbytes; ++i) {
                    if (!ram_.write_block(addr, nbytes, [](uint16_t
                }
            }
        */
        default:
            error();
    }
}

Reply Repl::parse_request(Request const& request)
{
    Reply reply(request.message_type(), buffer_);
    switch (request.message_type()) {
        case MessageType::FreeMem:
            reply.free_mem = free_ram();
            break;
        case MessageType::TestDebug:
            for (int i = 0; i < 3; ++i)
                serial_.debug_P(buffer_, PSTR("Debug message %d..."), i);
            break;
        case MessageType::TestDMA:
            memcpy(buffer_.data, ram_.test(), 6);
            buffer_.sz = 6;
            break;
        case MessageType::RamReadByte:
            reply.ram_byte = ram_.read_byte(request.ram_request.address);
            break;
        case MessageType::RamWriteByte:
            reply.ram_byte = ram_.write_byte(request.ram_request.address, request.ram_request.byte);
            break;
        case MessageType::RamReadBlock: {
                if (!ram_.read_block(request.ram_request.address, request.ram_request.size,
                           [](uint16_t idx, uint8_t byte, void* data) {
                                reinterpret_cast<Buffer*>(data)->data[idx] = byte;
                           }, &buffer_)) {
                    reply.result = Result::WrongChecksumDMA;
                }
            }
            break;
        case MessageType::RamWriteBlock: {
                if (!ram_.write_block(request.ram_request.address, request.ram_request.size,
                        [](uint16_t idx, void* data) {
                            return reinterpret_cast<Buffer*>(data)->data[idx];
                        }, &buffer_)) {
                    reply.result = Result::WrongChecksumDMA;
                }
            }
            break;
        default:
            reply.result = Result::InvalidRequest;
            break;
    }
    return reply;
}

Request Repl::recv_request(bool* status)
{
    *status = true;
    auto request = deserialize<Request>(buffer_, [](void* data) -> uint8_t {
        return ((Serial*) data)->recv();
    }, &serial_, true);
    if (request.deserialization_error() != DeserializationError::NoErrors)
        *status = false;
    return request;
}

void Repl::send_reply(Reply& reply)
{
    reply.serialize([](uint8_t byte, void* data) { ((Serial*) data)->send(byte); }, &serial_);
}

void Repl::do_message()
{
    bool status = true;
    Request request = recv_request(&status);

    Reply reply = parse_request(request);
    if (!status)
        reply.result = DeserializationErrorInController;
    send_reply(reply);
}

void Repl::execute()
{
    uint8_t cmd = getchar();
    if (cmd == MessageClass::MC_Request)
        do_message();
    else if (cmd >= ' ' && cmd <= '~')
        do_terminal(cmd);
    else
        serial_.send(InvalidCommand);
}
