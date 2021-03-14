#include "repl.hh"

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <avr/pgmspace.h>

#include <libf1comm/defines.hh>
#include <libf1comm/messages/request.hh>
#include <libf1comm/messages/reply.hh>
#include <libf1comm/messages/deserialize.hh>

#include "tests.hh"

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
#ifdef ENABLE_TESTS
    if (do_tests(cmd, fortuna1_, buffer_))
        return;
#endif
    switch (cmd) {
        case 'h':
        case '?':
            printf_P(PSTR("Ctrl:    [f] bytes free   [R] reset       [t] soft reset\n"));
            printf_P(PSTR("RAM:     [r] read byte    [w] write byte  [W] write multiple bytes  [d] dump memory\n"));
            printf_P(PSTR("SdCard:  [l] last status  [s] dump block\n"));
            printf_P(PSTR("Z80:     [i] CPU info     [p] step\n"));
            printf_P(PSTR("Low lvl: [b] buses state\n"));
#ifdef ENABLE_TESTS
            printf_P(tests_help());
#endif
            break;
        case 'f':
            printf_P(PSTR("%d bytes free.\n"), free_ram());
            break;
        case 't':
            fortuna1_.soft_reset();
            printf_P(PSTR("System soft reset.\n"));
            break;
        case 'R':
            fortuna1_.hard_reset(buffer_);
            printf_P(PSTR("System reset.\n"));
            break;
        case 'r': {
                int addr = ask_value_P(PSTR("Addr"));
                if (addr != ERROR)
                    printf_P(PSTR("0x%02X\n"), fortuna1_.read_byte(addr));
            }
            break;
        case 'w': {
                Values vv = ask_two_values_P(PSTR("Addr Data"));
                if (vv.v1 != ERROR) {
                    printf_P(PSTR("0x%02X\n"), fortuna1_.write_byte(vv.v1, vv.v2));
                }
            }
            break;
        case 'd': {
                int page = ask_value_P(PSTR("Page (0x100)"));
                if (page != ERROR) {
                    if (!fortuna1_.read_block(page * 0x100, 0x100, [](uint16_t addr, uint8_t byte, void* page) {
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
        case 'l':
            printf_P(PSTR("Last stage: 0x%02X   last response: 0x%02X\n"), fortuna1_.sdcard().last_stage(), fortuna1_.sdcard().last_response());
            break;
        case 's': {
                int block = ask_value_P(PSTR("Block"));
                if (block == ERROR)
                    return;
                if (!fortuna1_.sdcard().read_page(block, buffer_)) {
                    printf_P(PSTR("Error reading SDCard.\n"));
                    return;
                }
                for (size_t i = 0; i < 512; ++i) {
                    if (i % 0x10 == 0)
                        printf_P(PSTR("%08X : "), block * 512 + i);
                    printf_P(PSTR("%02X "), buffer_.data[i]);
                    if (i % 0x10 == 0xf)
                        putchar('\n');
                }
            }
            break;
        case 'i':
            printf_P(PSTR("Powered: %s   Cycle: %d   PC: 0x%04X\n"),
                     fortuna1_.z80().powered() ? "yes" : "no",
                     fortuna1_.z80().cycle_count(), fortuna1_.z80().pc());
            break;
        case 'p':
            fortuna1_.z80().step();
            printf_P(PSTR("PC = %04X\n"), fortuna1_.z80().pc());
            break;
        case 'b': {
                uint8_t data = fortuna1_.ram().data_bus();
                uint16_t addr = fortuna1_.ram().addr_bus();
                RAM::MemoryBus mbus = fortuna1_.ram().memory_bus();
                Z80Pins pins = fortuna1_.z80().pins();
                printf_P(PSTR("ADDR DATA  MREQ WR RD  INT NMI RST BUSRQ  HALT IORQ M1 BUSAK\n"));
                printf_P(PSTR("0000  %s     1   1  1   1   1   1    1      1    1   1    1\n"),
                        addr_s, data_s, bit(mbus.mreq), bit(mbus.wr), bit(mbus.rd),
                        bit(pins.int_), bit(pins.nmi), bit(pins.rst), bit(pins.busrq),
                        bit(pins.halt), bit(pins.iorq), bit(pins.m1), bit(pins.busak));
                break;
            }
            break;
        default:
            error();
    }
}

Reply Repl::parse_request(Request const& request)
{
    Reply reply(request.message_type(), buffer_);
    switch (request.message_type()) {
        case MessageType::SoftReset:
            fortuna1_.soft_reset();
            buffer_.sz = 0;
            break;
        case MessageType::HardReset:
            fortuna1_.hard_reset(buffer_);
            buffer_.sz = 0;
            break;
        case MessageType::FreeMem:
            reply.free_mem = free_ram();
            break;
        case MessageType::TestDebug:
            for (int i = 0; i < 3; ++i)
                serial_.debug_P(buffer_, PSTR("Debug message %d..."), i);
            break;
        case MessageType::TestDMA:
            memcpy(buffer_.data, fortuna1_.ram().test(), 6);
            buffer_.sz = 6;
            break;
        case MessageType::RamReadByte:
            reply.ram_byte = fortuna1_.read_byte(request.ram_request.address);
            break;
        case MessageType::RamWriteByte:
            reply.ram_byte = fortuna1_.write_byte(request.ram_request.address, request.ram_request.byte);
            break;
        case MessageType::RamReadBlock: {
                if (!fortuna1_.read_block(request.ram_request.address, request.ram_request.size,
                           [](uint16_t idx, uint8_t byte, void* data) {
                                reinterpret_cast<Buffer*>(data)->data[idx] = byte;
                           }, &buffer_)) {
                    reply.result = Result::WrongChecksumDMA;
                }
                buffer_.sz = request.ram_request.size;
            }
            break;
        case MessageType::RamWriteBlock: {
                if (!fortuna1_.write_block(request.ram_request.address, request.ram_request.size,
                        [](uint16_t idx, void* data) {
                            return reinterpret_cast<Buffer*>(data)->data[idx];
                        }, &buffer_)) {
                    reply.result = Result::WrongChecksumDMA;
                }
            }
            break;
        case MessageType::SDCard_Status:
            reply.sd_status = { static_cast<uint8_t>(fortuna1_.sdcard().last_stage()), fortuna1_.sdcard().last_response() };
            break;
        case MessageType::SDCard_Read:
            if (!fortuna1_.sdcard().read_page(request.sdcard_block, buffer_))
                reply.result = SDCardError;
            reply.sd_status = { static_cast<uint8_t>(fortuna1_.sdcard().last_stage()), fortuna1_.sdcard().last_response() };
            break;
        case MessageType::Z80_CpuInfo:
            reply.z80_info = { fortuna1_.z80().cycle_count(), fortuna1_.z80().pc() };
            break;
        case MessageType::Z80_Step:
            fortuna1_.z80().step();
            reply.z80_info = { fortuna1_.z80().cycle_count(), fortuna1_.z80().pc() };
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
