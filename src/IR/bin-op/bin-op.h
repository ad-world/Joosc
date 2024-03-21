#pragma once

#include <memory>
#include "IR/ir.h"
#include <string>
#include <variant>
#include "utillities/overload.h"

class BinOpIR {
    enum OpType {
        ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, LSHIFT, RSHIFT, ARSHIFT,
        EQ, NEQ, LT, GT, LEQ, GEQ
    };

    OpType op;
    std::unique_ptr<ExpressionIR> left;
    std::unique_ptr<ExpressionIR> right;
public:
    BinOpIR(OpType op, std::unique_ptr<ExpressionIR> left, std::unique_ptr<ExpressionIR> right) : op(op), left{std::move(left)}, right{std::move(right)} {}
    OpType opType() { return op; }
    ExpressionIR &getLeft() { assert(left.get()); return *left.get(); }
    ExpressionIR &getRight() { assert(right.get()); return *right.get(); }
    
    bool isConstant() {
        // bool first = std::visit([&](auto &x) { return x.isConstant(); }, *left);
        // bool second = std::visit([&](auto &x){ return x.isConstant(); }, *right);
        // return first && second;
        return false;
    }

};
