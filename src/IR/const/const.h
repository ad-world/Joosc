#pragma once

#include "IR/ir_variant.h"
#include <memory>
#include <string>

class ConstIR {
    int64_t value;
public:
    ConstIR(int64_t value) : value(value) {}
    int getValue() { return value; }
    std::string label() { return "CONST (" + std::to_string(value) + ")"; }
    bool isConstant() { return true; }

    static std::unique_ptr<ExpressionIR> makeZero();
    static std::unique_ptr<ExpressionIR> makeOne();
    static std::unique_ptr<ExpressionIR> makeWords(int num_words = 1);
    static std::unique_ptr<ExpressionIR> makeExpr(int64_t value);
};
