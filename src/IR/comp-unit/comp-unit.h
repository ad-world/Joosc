#pragma once

#include <string>
#include <memory>
#include <vector>
#include "IR/ir_variant.h"
#include "unordered_map"
#include "IR/func-decl/func-decl.h"

class CompUnitIR {
    std::string name;
    std::unordered_map<std::string, FuncDeclIR*> functions;     // These point to functions inside of child_functions
    std::vector<std::unique_ptr<FuncDeclIR>> child_functions;

    std::string class_name; // Used for graphing

public:
    // PROBABLY NOT NEEDED (causes errors)
    // CompUnitIR(std::string name, std::unordered_map<std::string, std::unique_ptr<FuncDeclIR>> functions) : name(name), functions(functions) {}
    CompUnitIR(std::string name) : name(name) {}
    void appendFunc(std::string name, std::unique_ptr<FuncDeclIR> func) {
        child_functions.emplace_back(std::move(func));
        functions[name] = child_functions.back().get();
    }
    FuncDeclIR* getFunc(std::string name) { 
        if (functions.find(name) == functions.end()) {
            int last_dot = name.find_last_of('.');
            if (last_dot != std::string::npos) {
                name = name.substr(last_dot + 1);
            }
        }
    
        return functions[name];
    }
    std::string getName() { return name; }
    std::unordered_map<std::string, FuncDeclIR*> getFunctions() { return functions; }
    std::string label() { return "COMPUNIT (" + class_name + ")"; }

    void setClassName(std::string name) { class_name = name; }
    std::vector<std::unique_ptr<FuncDeclIR>>& getFunctionList() { return child_functions; }
};
