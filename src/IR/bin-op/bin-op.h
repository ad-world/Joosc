#pragma once

#include <memory>
#include "IR/ir_variant.h"
#include <cassert>

class BinOpIR {
    std::unique_ptr<ExpressionIR> left;
    std::unique_ptr<ExpressionIR> right;
public:
    enum OpType {
        ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, LSHIFT, RSHIFT, ARSHIFT,
        EQ, NEQ, LT, GT, LEQ, GEQ
    };
    OpType op;

    BinOpIR(OpType op, std::unique_ptr<ExpressionIR> left, std::unique_ptr<ExpressionIR> right) : op(op), left{std::move(left)}, right{std::move(right)} {}
    ExpressionIR &getLeft() { assert(left.get()); return *left.get(); }
    ExpressionIR &getRight() { assert(right.get()); return *right.get(); }
    std::string label();
    
    bool isConstant();
    static std::unique_ptr<ExpressionIR> makeNegate(std::unique_ptr<ExpressionIR> negated);
    static std::unique_ptr<ExpressionIR> makeExpr(OpType op, std::unique_ptr<ExpressionIR> left, std::unique_ptr<ExpressionIR> right);
};
