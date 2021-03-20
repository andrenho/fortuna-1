#ifndef CONTROLLER_TERMINAL_HH
#define CONTROLLER_TERMINAL_HH

class Terminal {
public:
    uint8_t last_printed_char() const { return last_printed_char_; }
    void set_last_printed_char(uint8_t last_printed_char) { last_printed_char_ = last_printed_char; }

private:
    uint8_t last_printed_char_;
};

#endif
