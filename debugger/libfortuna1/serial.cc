#include "serial.hh"

#include <cstring>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <stdexcept>
#include <string>
#include <iostream>

using namespace std::string_literals;

#include "replyexception.hh"
#include "libf1comm/messages/debuginformation.hh"
#include "libf1comm/messages/deserialize.hh"

Serial::Serial(const char* port)
{
    fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        throw std::runtime_error("Could not open serial port: "s + strerror(errno));
    }
    
    // set interface attributes
    struct termios tty{};
    if (tcgetattr(fd, &tty) != 0) {
        throw std::runtime_error("Could not get terminal attributes: "s + strerror(errno));
    }
    cfsetospeed(&tty, B38400);
    cfsetispeed(&tty, B38400);
    cfmakeraw(&tty);
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 1;   // should block
    tty.c_cc[VTIME] = 0;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        throw std::runtime_error("Could not get terminal attributes: "s + strerror(errno));
    }
    
    std::cout << "Serial port connected.\n";
}

Serial::~Serial()
{
    close(fd);
}

Reply
Serial::request(Request const& request, Buffer& buffer) const
{
    send_request(request);
    
    Reply reply = receive_reply(buffer);
    // TODO - check for errors on reply
    return reply;
}

void
Serial::send_request(Request const& request) const
{
    if (log_message_ || log_bytes_)
        printf("\e[0;32m");
    
    if (log_message_)
        request.debug();
    
    std::string req_str = request.serialize_to_string();
    
    if (log_bytes_) {
        for (uint8_t c: req_str)
            printf("%02X ", c);
        printf("\n");
    }
    
    if (log_message_ || log_bytes_)
        printf("\e[0m");
    
    size_t sent = 0;
    size_t count = 0;
    while (sent < req_str.size()) {
        int n = write(fd, &req_str[sent], req_str.size() - sent);
        if (n == 0)
            throw std::runtime_error("When sending message, no bytes could be sent.");
        else if (n < 0)
            throw std::runtime_error("Error sending message: "s + strerror(errno));
        if (count > 255)
            throw std::runtime_error("Infinite loop trying to send message.");
        sent += n;
        ++count;
    }
}

static int check(int n) {
    if (n == 0)
        throw std::runtime_error("Unexpected end-of-file when receiving message");
    else if (n < 0)
        throw std::runtime_error("Error receiving message: "s + strerror(errno));
    return n;
}

Reply Serial::receive_reply(Buffer& buffer) const
{
next_message:
    if (log_message_ || log_bytes_)
        printf("\e[0;34m");
    
    uint8_t resp;
    check(read(fd, &resp, 1));
    if (log_bytes_) {
        printf("%02X ", resp);
        fflush(stdout);
    }
    
    switch (resp) {
        case MessageClass::MC_Reply: {
            Reply reply = parse_reply(buffer);
            if (log_message_ || log_bytes_)
                printf("\e[0m");
            return reply;
        }
        case MessageClass::MC_DebugInformation:
            parse_debug_information(buffer);
            if (log_message_ || log_bytes_)
                printf("\e[0m");
            goto next_message;
        default:
            if (log_message_ || log_bytes_)
                printf("\e[0m");
            throw ReplyException("Unexpected message class " + std::to_string(resp) + " instead of Reply of DebugInformation.");
    }
}

uint8_t Serial::input_byte(void* data) {
    auto* s = (Serial*) data;
    uint8_t byte;
    read(s->fd, &byte, 1);
    if (s->log_bytes_) {
        printf("%02X ", byte);
        fflush(stdout);
    }
    return byte;
}

void Serial::parse_debug_information(Buffer& buffer) const
{
    auto di = deserialize<DebugInformation>(buffer, Serial::input_byte, (void*) this, true);
    check_deserialization_error(di);
    printf("\e[0;31m%s\e[0m\n", (const char*) buffer.data);
}

Reply Serial::parse_reply(Buffer& buffer) const
{
    auto reply = deserialize<Reply>(buffer, Serial::input_byte, (void*) this, true);
    if (log_message_)
        reply.debug();
    
    check_deserialization_error(reply);
    
    switch (reply.result) {
        case Result::OK:
            return reply;
        case Result::InvalidRequest:
            throw ReplyException("The controller reported that we sent a invalild request.");
        case Result::WrongChecksumDMA:
            throw ReplyException("The controller reported that the DMA sent a invalid checksum during an operation.");
        case Result::DeserializationErrorInController:
            throw ReplyException("The controller reported that it had a deserialization error.");
        default: {
            char b[3];
            sprintf(b, "%02X", reply.result);
            throw ReplyException("Invalid byte 0x"s + b + " received from controller.");
        }
    }
}

void Serial::check_deserialization_error(Message const& message)
{
    switch (message.deserialization_error()) {
        case DeserializationError::InvalidMessageClass:
            throw ReplyException("Invalid message class when deserializing reply.");
        case DeserializationError::ChecksumDoesNotMatch:
            throw ReplyException("Checksum does not match when deserializing reply.");
        case DeserializationError::FinalByteNotReceived:
            throw ReplyException("Final byte (0xe4) not received when deserializing reply.");
        case DeserializationError::BufferDataTooLarge:
            throw ReplyException("Buffer data too large when deserializing reply.");
        case DeserializationError::NoErrors:
            break;
    }
}
