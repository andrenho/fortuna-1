#ifndef LIBFORTUNA1_FORTUNA1_HH
#define LIBFORTUNA1_FORTUNA1_HH

class Fortuna1 {
public:
    virtual ~Fortuna1() = default;
    virtual size_t free_mem() const = 0;
    
    void set_log_bytes(bool v) { log_bytes_ = v; }
    void set_log_messages(bool v) { log_messages_ = v; }

protected:
    Fortuna1() = default;
    
    bool log_bytes_ = false;
    bool log_messages_ = false;
};

#endif
