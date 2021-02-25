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

#include "protocol.h"
#include "replyexception.hh"

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
Serial::request(Request const& request) const
{
    send_request(request);
    Reply reply = receive_reply();
    if (reply.type() != request.type()) {
        throw ReplyException("The reply type (" + std::to_string(reply.type()) + ") does not match with the request type (" + std::to_string(request.type()) + ")");
    } else if (reply.result() != reply.result()) {
        if (reply.additionalinfo().empty())
            throw ReplyException(reply.result(), reply.additionalinfo());
        else
            throw ReplyException(reply.result());
    }
    return reply;
}

void
Serial::send_request(Request const& request) const
{
    std::string data;
    request.SerializeToString(&data);
    
    std::string req_str = std::string(1, Z_FOLLOWS_PROTOBUF_REQ) + length_string_16(data) + data
            + checksum_string(data) + std::string(1, Z_REQUEST_OVER);
    
    if (log_message_ || log_bytes_)
        printf("\e[0;32m");
    if (log_message_)
        printf("%s", request.DebugString().c_str());
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

Reply Serial::receive_reply() const
{
    auto check = [](int n) -> int {
        if (n == 0)
            throw std::runtime_error("Unexpected end-of-file when receiving message");
        else if (n < 0)
            throw std::runtime_error("Error receiving message: "s + strerror(errno));
        return n;
    };
    
    if (log_message_ || log_bytes_)
        printf("\e[0;31m");
    
    // get response
    uint8_t resp;
get_new_response:
    check(read(fd, &resp, 1));
    if (log_bytes_) {
        printf("%02X ", resp);
        fflush(stdout);
    }
    
    // print debug message?
    if (resp == Z_FOLLOWS_DEBUG_MSG) {
        uint8_t c;
        while (true) {
            check(read(fd, &c, 1));
            if (c == 0)
                break;
            putchar(c);
        }
        printf("\n");
        fflush(stdout);
        goto get_new_response;
    }
    
    // check for errors
    if (resp == Z_CHECKSUM_NO_MATCH) {
        throw ReplyException("Controller informed that checksum sent does not match.");
    } else if (resp == Z_REQUEST_TOO_LARGE) {
        throw ReplyException("Controller informed that message sent is too large.");
    } else if (resp == Z_RESPONSE_TOO_LARGE) {
        throw ReplyException("Controller informed that the response would be too large to create.");
    } else if (resp == Z_REQUEST_NOT_OVER) {
        throw ReplyException("Expected request over but received a different byte.");
    } else if (resp == Z_ERROR_DECODING_REQUEST) {
        throw ReplyException("Controller informed error decoding the request.");
    } else if (resp == Z_ERROR_ENCODING_REPLY) {
        throw ReplyException("Controller informed error encoding the reply.");
    } else if (resp == Z_INVALID_COMMAND) {
        throw ReplyException("Controller reported invalid command.");
    } else if (resp != Z_FOLLOWS_PROTOBUF_RESP) {
        char buf[3]; sprintf(buf, "%02X", resp);
        throw ReplyException("Unexpected response from controller: "s + buf);
    }
    
    // get message size
    uint8_t ssz[2];
    check(read(fd, ssz, 2));
    if (log_bytes_) {
        printf("%02X %02X ", ssz[0], ssz[1]);
        fflush(stdout);
    }
    uint16_t msg_sz = (ssz[0] << 8) | ssz[1];
    
    // receive message
    std::string buffer(msg_sz, 0);
    size_t received = 0;
    while (received < msg_sz)
        received -= check(read(fd, &buffer[received], msg_sz - received));
    if (log_bytes_) {
        for (uint8_t c: buffer) {
            printf("%02X ", c);
            fflush(stdout);
        }
    }
    
    // get checksum and calculate it
    check(read(fd, ssz, 2));
    if (log_bytes_) {
        printf("%02X %02X ", ssz[0], ssz[1]);
        fflush(stdout);
    }
    auto [sum1, sum2] = checksum(buffer);
    
    // get reply over
    check(read(fd, &resp, 1));
    if (log_bytes_) {
        printf("%02X ", resp);
        fflush(stdout);
    }
    if (resp != Z_REPLY_OVER)
        throw ReplyException("Expected reply over but received a different byte.");
    
    if (ssz[0] != sum2 || ssz[1] != sum1)
        throw std::runtime_error("Invalid checksum in message sent by controller.");
    
    Reply reply;
    reply.ParseFromString(buffer);
    if (log_message_)
        printf("%s", reply.DebugString().c_str());
    
    if (log_message_ || log_bytes_)
        printf("\e[0m");
    return reply;
}

std::string Serial::length_string_16(std::string const& data)
{
    return std::string(1, (data.size() >> 8) & 0xff) + static_cast<char>(data.size() & 0xff);
}

std::pair<uint8_t, uint8_t> Serial::checksum(std::string const& data)
{
    // https://en.wikipedia.org/wiki/Fletcher%27s_checksum
    uint16_t sum1 = 0, sum2 = 0;
    for (uint8_t c: data) {
        sum1 = (sum1 + c) % 0xff;
        sum2 = (sum2 + sum1) % 0xff;
    }
    return { sum1, sum2 };
}

std::string Serial::checksum_string(std::string const& data)
{
    auto [sum1, sum2] = checksum(data);
    return std::string(1, sum2) + static_cast<char>(sum1);
}

