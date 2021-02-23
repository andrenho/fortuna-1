#ifndef LIBFORTUNA1_REPLYEXCEPTION_HH
#define LIBFORTUNA1_REPLYEXCEPTION_HH

#include <stdexcept>

class ReplyException : public std::runtime_error {
public:
    explicit ReplyException(const char* what) : std::runtime_error(what) {}
    explicit ReplyException(std::string const& what) : std::runtime_error(what) {}
    
    explicit ReplyException(Result result)
        : std::runtime_error("Error receiving message: " + translate_message(result)) {}
    
    explicit ReplyException(Result result, std::string const& additional_info)
        : std::runtime_error("Error receiving message: " + translate_message(result) + " [" + additional_info + "]") {}

private:
    static std::string translate_message(Result result) {
        switch (result) {
            default:
                return "Error message not implemented for result type " + std::to_string(result);
        }
    }
};

#endif
