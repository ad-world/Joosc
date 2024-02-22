#pragma once
#include <stdexcept>
#include <string>
using namespace std;

#define make_message(error_type, message, file, line) \
    file + ":" + to_string(line) + ":" + error_type + ": " + message

/**************************************
 *      ERRORS within our compiler
 **************************************/

// on catch, do not return valid or invalid exit code ( return something else )
#define THROW_CompilerError(message) throw CompilerError(message, __FILE__, __LINE__)
class CompilerError : runtime_error {
public:
    string message;
    CompilerError(string& message) : runtime_error("Compiler error: " + message) {}
    CompilerError(string& message, string& file, int line)
        : runtime_error(make_message("Compiler error", message, file, line)) {}
};



/************************************************
 *  ERRORS within the supplied java program
 ************************************************/

#define THROW_LexerError(message) throw LexerError(message, __FILE__, __LINE__)
class LexerError : runtime_error {
public:
    string message;
    LexerError(string& message) : runtime_error("Lexer error: " + message) {}
    LexerError(string& message, string& file, int line)
        : runtime_error(make_message("Lexer error", message, file, line)) {}
};


#define THROW_ParserError(message) throw LexerError(message, __FILE__, __LINE__)
class ParserError : runtime_error {
public:
    string message;
    ParserError(string& message) : runtime_error("Parser error: " + message) {}
    ParserError(string& message, string& file, int line)
        : runtime_error(make_message("Parser error", message, file, line)) {}
};

#define THROW_WeederError(message) throw WeederError(message, __FILE__, __LINE__)
class WeederError : runtime_error {
public:
    string message;
    WeederError(string& message) : runtime_error("Weeder error: " + message) {}
    WeederError(string& message, string& file, int line)
        : runtime_error(make_message("Weeder error", message, file, line)) {}
};

#define THROW_EnvBuilderError(message) throw EnvBuilderError(message, __FILE__, __LINE__)
class EnvBuilderError : runtime_error {
public:
    string message;
    EnvBuilderError(string& message) : runtime_error("EnvBuilder error: " + message) {}
    EnvBuilderError(string& message, string& file, int line)
        : runtime_error(make_message("EnvBuilder error", message, file, line)) {}
};

#define THROW_TypeLinkerError(message) throw TypeLinkerError(message, __FILE__, __LINE__)
class TypeLinkerError : runtime_error {
public:
    string message;
    TypeLinkerError(string& message) : runtime_error("TypeLinker error: " + message) {}
    TypeLinkerError(string& message, string& file, int line)
        : runtime_error(make_message("TypeLinker error", message, file, line)) {}
};

#define THROW_HierarchyError(message) throw HierarchyError(message, __FILE__, __LINE__)
class HierarchyError : runtime_error {
public:
    string message;
    HierarchyError(string& message) : runtime_error("HierarchyChecker error: " + message) {}
    HierarchyError(string& message, string& file, int line)
        : runtime_error(make_message("HierarchyChecker error", message, file, line)) {}
};
