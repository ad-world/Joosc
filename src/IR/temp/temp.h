#pragma once

#include "IR/ir_variant.h"
#include <memory>
#include <string>

class TempIR {
    static size_t num_temps;
    std::string name;

public:
    TempIR(std::string name) : name{std::move(name)} {}

    std::string &getName() { return name; }
    static std::string generateName(std::string prefix = "") {
        num_temps++;
        return (prefix.empty() ? "temp" : prefix) + std::to_string(num_temps);
    }

    std::string label() { return "TEMP(" + name + ")"; }
    bool isConstant() { return false; }

    static std::unique_ptr<ExpressionIR> makeExpr(std::string str);
};
