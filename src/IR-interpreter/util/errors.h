#include <stdexcept>

struct Trap : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct InternalCompilerError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
