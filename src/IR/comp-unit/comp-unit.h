#include <string>
#include "IR/ir.h"
#include "unordered_map"

struct FuncDeclIR;

class CompUnitIR {
    std::string name;
    std::unordered_map<std::string, FuncDeclIR*> functions;

public:
    CompUnitIR(std::string name, std::unordered_map<std::string, FuncDeclIR*> functions) : name(name), functions(functions) {}
    CompUnitIR(std::string name) : name(name) {}
    void appendFunc(std::string name, FuncDeclIR* func) { functions[name] = func; }
    std::string name() { return name; }
    std::unordered_map<std::string, FuncDeclIR*> functions() { return functions; }
    std::string label() { return "COMPUNIT"; }
};