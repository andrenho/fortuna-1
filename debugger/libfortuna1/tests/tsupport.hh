#ifndef LIBFORTUNA1_TSUPPORT_HH
#define LIBFORTUNA1_TSUPPORT_HH

#include <memory>
#include <fortuna1.hh>

struct TestArgs {
   TestArgs(int argc, char** argv);
   
   std::unique_ptr<Fortuna1> create_fortuna() const;
   
   bool real_hardware_mode = false;
   std::string serial_port = "";
   bool log_bytes = false;
   bool log_messages = false;

private:
    static void show_help(const char* program) ;
};

#define ASSERT_GT(msg, value, greater_than) \
    std::cout << msg << "... "; \
    if (value > greater_than) { std::cout << "\e[0;32m✔\e[0m\n"; } else { std::cout << "\e[0;31mX\e[0m   Expected received value of " << value << " to be > " << greater_than << ".\n"; exit(1); }

#define ASSERT_EQ(msg, expected, received) \
    std::cout << msg << "... "; \
    if (expected == received) { std::cout << "\e[0;32m✔\e[0m\n"; } else { std::cout << "\e[0;31mX\e[0m   Expected: " << expected << "  Received: " << received << ".\n"; exit(1); }

#endif //LIBFORTUNA1_TSUPPORT_HH