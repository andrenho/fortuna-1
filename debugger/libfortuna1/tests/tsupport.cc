#include "tsupport.hh"

#include <getopt.h>

#include <fortuna1realhardware.hh>
#include <fortuna1emulator.hh>
#include <iostream>
#include <iomanip>

std::ostream& operator<<(std::ostream& os, std::vector<uint8_t> const& bytes)
{
    os << std::hex << std::setw(2) << std::setfill('0');
    for (uint8_t byte: bytes)
        os << (int) byte << ' ';
    return os;
}

TestArgs::TestArgs(int argc, char** argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "hbmr:")) != -1) {
        switch (opt) {
            case 'r':
                real_hardware_mode = true;
                serial_port = optarg;
                break;
            case 'b':
                log_bytes = true;
                break;
            case 'm':
                log_messages = true;
                break;
            case 'h':
                show_help(argv[0]);
                exit(EXIT_SUCCESS);
            default:
                show_help(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}

std::unique_ptr<Fortuna1> TestArgs::create_fortuna() const
{
    std::unique_ptr<Fortuna1> f;
    if (real_hardware_mode)
        f = std::make_unique<Fortuna1RealHardware>(serial_port);
    else
        f = std::make_unique<Fortuna1Emulator>();
    f->set_log_bytes(log_bytes);
    f->set_log_messages(log_messages);
    return f;
}

void TestArgs::show_help(const char* program)
{
    std::cout << "Usage: " << program << "\n";
    std::cout << "    -r SERIAL_PORT     Use real hardware\n";
    std::cout << "    -h                 This help\n";
    std::cout << "    -b                 Log bytes\n";
    std::cout << "    -m                 Log messages\n";
}
