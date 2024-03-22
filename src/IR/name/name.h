#pragma once

#include "IR/ir_variant.h"
#include <memory>
#include <string>

class NameIR {
    std::string name; 

public:
    NameIR(std::string name) : name{std::move(name)} {}
    std::string &getName() { return name; }
    std::string label() { return "NAME(" + name + ")"; }
    bool isConstant() { return false; }

    static std::unique_ptr<ExpressionIR> makeMalloc();
    static std::unique_ptr<ExpressionIR> makeException();
};
