#pragma once

#include <string>
#include "IR/ir.h"
#include "unordered_map"
#include "IR/func-decl/func-decl.h"


class CompUnitIR {
    std::string name;
    std::unordered_map<std::string, std::unique_ptr<FuncDeclIR>> functions;

public:
    CompUnitIR(std::string name, std::unordered_map<std::string, std::unique_ptr<FuncDeclIR>> functions) : name(name), functions(functions) {}
    CompUnitIR(std::string name) : name(name) {}
    void appendFunc(std::string name, std::unique_ptr<FuncDeclIR> func) { functions[name] = std::move(func); }
    std::string getName() { return name; }
    std::unordered_map<std::string, std::unique_ptr<FuncDeclIR>> getFunctions() { return functions; }
    std::string label() { return "COMPUNIT"; }
};
