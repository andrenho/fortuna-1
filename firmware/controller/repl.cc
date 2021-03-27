#include "repl.hh"

#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <libf1comm/defines.hh>
#include <libf1comm/messages/request.hh>
#include <libf1comm/messages/reply.hh>
#include <libf1comm/messages/deserialize.hh>

Reply Repl::parse_request(Request const& request)
{
    wdt_enable(WDTO_2S);
    Reply reply(request.message_type(), buffer_);
    wdt_disable();

    fortuna1_.z80().set_debug_mode(false);

    switch (request.message_type()) {
        case MessageType::SoftReset:
            reply.reset_status = fortuna1_.soft_reset();
            buffer_.sz = 0;
            break;
        case MessageType::HardReset:
            reply.reset_status = fortuna1_.hard_reset(buffer_);
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
            reply.z80_info = { fortuna1_.z80().cycle_count(), fortuna1_.z80().pc(), fortuna1_.terminal().last_printed_char() };
            break;
        case MessageType::Z80_Step:
            fortuna1_.z80().step();
            reply.z80_info = { fortuna1_.z80().cycle_count(), fortuna1_.z80().pc(), fortuna1_.terminal().last_printed_char() };
            break;
        case MessageType::Z80_NMI:
            fortuna1_.z80().nmi();
            reply.z80_info = { fortuna1_.z80().cycle_count(), fortuna1_.z80().pc(), fortuna1_.terminal().last_printed_char() };
            break;
        case MessageType::Keypress:
            fortuna1_.terminal().keypress(request.keypress);
            break;
        case MessageType::BreakpointsList:
            add_breakpoints_to_buffer(fortuna1_.breakpoints(), buffer_);
            break;
        case MessageType::BreakpointsAdd:
            if (!fortuna1_.breakpoints().add(request.address))
                reply.result = TooManyBreakpoints;
            add_breakpoints_to_buffer(fortuna1_.breakpoints(), buffer_);
            break;
        case MessageType::BreakpointsRemove:
            fortuna1_.breakpoints().remove(request.address);
            add_breakpoints_to_buffer(fortuna1_.breakpoints(), buffer_);
            break;
        case MessageType::BreakpointsRemoveAll:
            fortuna1_.breakpoints().remove_all();
            add_breakpoints_to_buffer(fortuna1_.breakpoints(), buffer_);
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
        console_.execute(cmd);
    else if (cmd == SYSTEM_RESET)
        fortuna1_.system_reset();
    else
        serial_.send(InvalidCommand);
}

void Repl::add_breakpoints_to_buffer(Breakpoints const& breakpoints, Buffer& buffer)
{
    buffer_.sz = MAX_BREAKPOINTS * 2;
    for (size_t i = 0; i < MAX_BREAKPOINTS; ++i) {
        uint16_t addr = breakpoints.list()[i];
        buffer.data[i * 2] = addr & 0xff;
        buffer.data[i * 2 + 1] = addr >> 8;
    }
}

