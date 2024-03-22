#pragma once

#include "IR/ir_variant.h"
#include <memory>
#include <string>

class ConstIR {
    int value;
public:
    ConstIR(int value) : value(value) {}
    int getValue() { return value; }
    std::string label() { return "CONST"; }
    bool isConstant() { return true; }
    static std::unique_ptr<ExpressionIR> makeZero();
    static std::unique_ptr<ExpressionIR> makeOne();
};
