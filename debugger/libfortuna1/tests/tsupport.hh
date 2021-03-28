#ifndef LIBFORTUNA1_TSUPPORT_HH
#define LIBFORTUNA1_TSUPPORT_HH

#include <iostream>
#include <memory>
#include <set>

#include <fortuna1.hh>

struct TestArgs {
   TestArgs(int argc, char** argv);
   
   std::unique_ptr<Fortuna1> create_fortuna();
   Z80_Info run_code(std::string const& code, size_t num_steps = 0);
   
   bool real_hardware_mode = false;
   std::string serial_port = "";
   bool log_bytes = false;
   bool log_messages = false;

private:
    static void show_help(const char* program);
    Fortuna1* fortuna1_ = nullptr;
};

std::ostream& operator<<(std::ostream& os, std::vector<uint8_t> const& bytes);

template <typename T>
void assert_gt(std::string const& description, T&& value, T&& greater_than) {
    std::cout << description << "... ";
    if (value > greater_than) {
        std::cout << "\e[0;32m✔\e[0m\n";
    } else {
        std::cout << "\e[0;31mX\e[0m   Expected received value of " << value << " to be > " << greater_than << "\n";
        exit(1);
    }
}

template <typename T, typename U>
void assert_eq(std::string const& description, T&& expected, U&& received)
{
    std::cout << description << "... ";
    if (std::tie(expected) == std::tie(received)) {
        std::cout << "\e[0;32m✔\e[0m\n";
    } else {
        std::cout << "\e[0;31mX\e[0m   Expected: " << expected << "  Received: " << received << "\n";
        exit(1);
    }
}

template <typename T, typename U>
void assert_eq(T&& expected, U&& received)
{
    if (std::tie(expected) == std::tie(received)) {
        std::cout << "\e[0;32m✔\e[0m";
    } else {
        std::cout << "\e[0;31mX\e[0m   Expected: " << expected << "  Received: " << received << "\n";
        exit(1);
    }
}

template <typename T, typename U>
void assert_contains(std::string const& description, T&& expected, U&& received) {
    std::cout << description << "... ";
    std::set a(expected.begin(), expected.end()),
             b(expected.begin(), expected.end());
    if (a == b) {
        std::cout << "\e[0;32m✔\e[0m\n";
    } else {
        std::cout << "\e[0;31mX\e[0m   Containers do not match.\n";
        exit(1);
    }
}


template <typename F>
void assert_throws(std::string const& description, F f) {
    std::cout << description << "... ";
    try {
        f();
        std::cout << "\e[0;31mX\e[0m   Function did not throw.\n";
        exit(1);
    } catch (...) {
        std::cout << "\e[0;32m✔\e[0m\n";
    }
}

void title(std::string const& text);

#endif //LIBFORTUNA1_TSUPPORT_HH