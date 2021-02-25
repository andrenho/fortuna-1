#ifndef LIBFORTUNA1_FORTUNA1_HH
#define LIBFORTUNA1_FORTUNA1_HH

#include <string>

class Fortuna1 {
public:
    virtual ~Fortuna1() = default;
    
    virtual size_t free_mem() const = 0;
    
    virtual void test_debug_messages() const = 0;
    virtual std::string test_dma() const = 0;
    
    virtual void set_log_bytes(bool) {}
    virtual void set_log_messages(bool) {}

protected:
    Fortuna1() = default;
};

#endif
