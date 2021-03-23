#include "tsupport.hh"

#include <iostream>
#include <iomanip>
#include <getopt.h>
#include <unistd.h>

#include "realhardware/fortuna1realhardware.hh"
#include "emulator/fortuna1emulator.hh"
#include "compiler/compiler.hh"

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

std::unique_ptr<Fortuna1> TestArgs::create_fortuna()
{
    std::unique_ptr<Fortuna1> f;
    if (real_hardware_mode) {
        f = std::make_unique<Fortuna1RealHardware>(serial_port);
    } else {
        f = std::make_unique<Fortuna1Emulator>();
        dynamic_cast<Fortuna1Emulator*>(f.get())->sdcard_set_image("../tests/sdcard.img");
    }
    fortuna1_ = f.get();
    f->set_log_bytes(log_bytes);
    f->set_log_messages(log_messages);
    f->system_reset();
    std::cout << "System reset.\n";
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

Z80_Info TestArgs::run_code(std::string const& code, size_t num_steps)
{
    std::string filename = "/tmp/testcode.z80";
    std::ofstream file(filename);
    file.write(code.data(), code.size());
    file.close();
    
    CompilerResult r = compile({ { filename, 0x0 } });
    if (r.error.has_value())
        throw std::runtime_error(r.error.value());
    
    if (log_messages) {
        std::cout << "Compiled code: ";
        for (uint8_t b: r.binaries.at("testcode.z80").data)
            std::cout << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << (int) b << ' ';
        std::cout << '\n';
    }
    
    fortuna1_->ram_write_buffer(0x0, r.binaries.at("testcode.z80").data);
    fortuna1_->soft_reset();
    Z80_Info info {};
    for (size_t i = 0; i < num_steps; ++i)
        info = fortuna1_->z80_step();
    
    unlink("/tmp/testcode.z80");
    return info;
}

void title(std::string const& text)
{
    std::cout << "\e[0;31m";
    std::cout << "--\n";
    std::cout << "-- " << text << "\n";
    std::cout << "--\n";
    std::cout << "\e[0m";
}