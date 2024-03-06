#pragma once
#include <stdexcept>
#include <string>

#define make_message(error_type, message, file, line) \
    file + ":" + std::to_string(line) + ":" + error_type + ": " + message

/**************************************
 *      ERRORS within our compiler
 **************************************/

// on catch, do not return valid or invalid exit code ( return something else )
#define THROW_CompilerError(message) throw CompilerError(message, __FILE__, __LINE__)
class CompilerError : public std::runtime_error {
public:
    std::string message;
    CompilerError(std::string message) : runtime_error("Compiler error: " + message) {}
    CompilerError(std::string message, std::string file, int line)
        : runtime_error(make_message("Compiler error", message, file, line)) {}
};



/************************************************
 *  ERRORS within the supplied java program
 ************************************************/

#define THROW_LexerError(message) throw LexerError(message, __FILE__, __LINE__)
class LexerError : public std::runtime_error {
public:
    std::string message;
    LexerError(std::string message) : runtime_error("Lexer error: " + message) {}
    LexerError(std::string message, std::string file, int line)
        : runtime_error(make_message("Lexer error", message, file, line)) {}
};


#define THROW_ParserError(message) throw LexerError(message, __FILE__, __LINE__)
class ParserError : public std::runtime_error {
public:
    std::string message;
    ParserError(std::string message) : runtime_error("Parser error: " + message) {}
    ParserError(std::string message, std::string file, int line)
        : runtime_error(make_message("Parser error", message, file, line)) {}
};

#define THROW_WeederError(message) throw WeederError(message, __FILE__, __LINE__)
class WeederError : public std::runtime_error {
public:
    std::string message;
    WeederError(std::string message) : runtime_error("Weeder error: " + message) {}
    WeederError(std::string message, std::string file, int line)
        : runtime_error(make_message("Weeder error", message, file, line)) {}
};

#define THROW_EnvBuilderError(message) throw EnvBuilderError(message, __FILE__, __LINE__)
class EnvBuilderError : public std::runtime_error {
public:
    std::string message;
    EnvBuilderError(std::string message) : runtime_error("EnvBuilder error: " + message) {}
    EnvBuilderError(std::string message, std::string file, int line)
        : runtime_error(make_message("EnvBuilder error", message, file, line)) {}
};

#define THROW_TypeLinkerError(message) throw TypeLinkerError(message, __FILE__, __LINE__)
class TypeLinkerError : public std::runtime_error {
public:
    std::string message;
    TypeLinkerError(std::string message) : runtime_error("TypeLinker error: " + message) {}
    TypeLinkerError(std::string message, std::string file, int line)
        : runtime_error(make_message("TypeLinker error", message, file, line)) {}
};

#define THROW_HierarchyError(message) throw HierarchyError(message, __FILE__, __LINE__)
class HierarchyError : public std::runtime_error {
public:
    std::string message;
    HierarchyError(std::string message) : runtime_error("HierarchyChecker error: " + message) {}
    HierarchyError(std::string message, std::string file, int line)
        : runtime_error(make_message("HierarchyChecker error", message, file, line)) {}
};

#define THROW_TypeCheckerError(message) throw TypeCheckError(message, __FILE__, __LINE__)
class TypeCheckError : public std::runtime_error {
public:
    std::string message;
    TypeCheckError(std::string message) : runtime_error("TypeCheckError error: " + message) {}
    TypeCheckError(std::string message, std::string file, int line)
        : runtime_error(make_message("TypeCheckError error", message, file, line)) {}
};
