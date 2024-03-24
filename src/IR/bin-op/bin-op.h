#pragma once

#include <memory>
#include "IR/ir_variant.h"
#include <cassert>

class BinOpIR {
public:
    enum OpType {
        ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, LSHIFT, RSHIFT, ARSHIFT,
        EQ, NEQ, LT, GT, LEQ, GEQ
    };
private:

    OpType op;
    std::unique_ptr<ExpressionIR> left;
    std::unique_ptr<ExpressionIR> right;
public:
    BinOpIR(OpType op, std::unique_ptr<ExpressionIR> left, std::unique_ptr<ExpressionIR> right) : op(op), left{std::move(left)}, right{std::move(right)} {}
    OpType opType() { return op; }
    ExpressionIR &getLeft() { assert(left.get()); return *left.get(); }
    ExpressionIR &getRight() { assert(right.get()); return *right.get(); }
    std::string label();
    
    bool isConstant();
    static std::unique_ptr<ExpressionIR> makeExpr(OpType op, std::unique_ptr<ExpressionIR> left, std::unique_ptr<ExpressionIR> right);
};
