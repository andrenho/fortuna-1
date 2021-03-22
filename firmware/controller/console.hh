#ifndef CONTROLLER_CONSOLE_HH
#define CONTROLLER_CONSOLE_HH

#include "fortuna1.hh"

class Console {
public:
    Console(Serial& serial, Fortuna1& fortuna_1, Buffer& buffer) : serial_(serial), fortuna1_(fortuna_1), buffer_(buffer) {}
    
    void execute(char cmd);
    
private:
    Serial&   serial_;
    Fortuna1& fortuna1_;
    Buffer&   buffer_;
    
    void print_reset_status(ResetStatus r) const;
    
    struct Question {
        const char* question;
        uint8_t     size;
        uint32_t    response = 0;
    
        Question(char const* question, uint8_t size) : question(question), size(size) {}
    };
    static bool ask_question_P(Question* question, size_t n_questions);
    static bool ask_question_P(const char* question, uint8_t size, uint32_t* response);
};


#endif
