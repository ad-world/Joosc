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
    std::string name() { return name; }
    std::unordered_map<std::string, std::unique_ptr<FuncDeclIR>> functions() { return functions; }
    std::string label() { return "COMPUNIT"; }
};