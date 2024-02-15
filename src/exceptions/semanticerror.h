#include <string>

struct SemanticError {
    std::string message;
    virtual ~SemanticError() = default;
    SemanticError(std::string message) : message{message} {}
};
