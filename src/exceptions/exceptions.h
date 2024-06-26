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
 *  ERRORS within the simulator
 ************************************************/
#define THROW_SimulatorError(message) throw SimulatorError(message, __FILE__, __LINE__)
class SimulatorError : public std::runtime_error {
   public:
    std::string message;
    bool soft = false;
    SimulatorError(std::string message) : runtime_error("Simulator error: " + message) {}
    SimulatorError(std::string message, std::string file, int line, bool soft=false)
        : soft{soft}, runtime_error(make_message("Simulator error", message, file, line)) {}
};

// Used for errors that need not be caught
#define THROW_SoftSimulatorError(message) throw SimulatorError(message, __FILE__, __LINE__, true)

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

#define THROW_DisambiguationError(message) throw DisambiguationError(message, __FILE__, __LINE__)
class DisambiguationError : public std::runtime_error {
public:
    std::string message;
    DisambiguationError(std::string message) : runtime_error("Disambiguation error: " + message) {}
    DisambiguationError(std::string message, std::string file, int line)
        : runtime_error(make_message("Disambiguation error", message, file, line)) {}
};

#define THROW_TypeCheckerError(message) throw TypeCheckerError(message, __FILE__, __LINE__)
class TypeCheckerError : public std::runtime_error {
public:
    std::string message;
    TypeCheckerError(std::string message) : runtime_error("TypeChecker error: " + message) {}
    TypeCheckerError(std::string message, std::string file, int line)
        : runtime_error(make_message("TypeChecker error", message, file, line)) {}
};

#define THROW_ReachabilityError(message) throw ReachabilityError(message, __FILE__, __LINE__)
class ReachabilityError : public std::runtime_error {
public:
    std::string message;
    ReachabilityError(std::string message) : runtime_error("Reachability error: " + message) {}
    ReachabilityError(std::string message, std::string file, int line)
        : runtime_error(make_message("Reachability error", message, file, line)) {}
};

#define THROW_LocalVariableError(message) throw LocalVariableError(message, __FILE__, __LINE__)
class LocalVariableError : public std::runtime_error {
public:
    std::string message;
    LocalVariableError(std::string message) : runtime_error("LocalVariable error: " + message) {}
    LocalVariableError(std::string message, std::string file, int line)
        : runtime_error(make_message("LocalVariable error", message, file, line)) {}
};

#define THROW_ASTtoIRError(message) throw ASTtoIRError(message, __FILE__, __LINE__)
class ASTtoIRError : public std::runtime_error {
public:
    std::string message;
    ASTtoIRError(std::string message) : runtime_error("Ast to IR error: " + message) {}
    ASTtoIRError(std::string message, std::string file, int line)
        : runtime_error(make_message("Ast to IR error", message, file, line)) {}
};

#ifdef LIBFUZZER
#define THROW_LibfuzzerError() int *ptr = nullptr; int crash = *ptr
#endif
