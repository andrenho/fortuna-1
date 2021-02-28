#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdint.h>

class Serial {
public:
    struct Checksum {
        uint16_t sum1, sum2;
    };

    static Serial init();

    void     send(uint8_t byte) const;
    uint8_t  recv() const;
    uint16_t recv16() const;
    uint8_t  recv_noblock() const;

    void     reset_checksum();
    void     add_to_checksum(uint8_t data);
    bool     compare_checksum(uint8_t sum1, uint8_t sum2) const;
    Checksum checksum() const;

    void     clrscr() const;
    void     set_echo(bool v);

    void     debug_P(const char* fmt, ...) const;

private:
    Serial() = default;

    uint16_t sum1_ = 0,
             sum2_ = 0;
};

#endif
