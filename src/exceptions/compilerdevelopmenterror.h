#pragma once

#include <string>

struct CompilerDevelopmentError {
    std::string message;
    virtual ~CompilerDevelopmentError() = default;
    CompilerDevelopmentError(std::string message) : message{message} {}
};
