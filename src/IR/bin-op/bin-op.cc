#include "bin-op.h"
#include "IR/ir.h"
#include <memory>
#include <utility>

bool BinOpIR::isConstant() {
    bool first = std::visit([&](auto &x) { return x.isConstant(); }, *left);
    bool second = std::visit([&](auto &x){ return x.isConstant(); }, *right);
    return first && second;
}

std::unique_ptr<ExpressionIR> BinOpIR::makeExpr(OpType op, std::unique_ptr<ExpressionIR> left, std::unique_ptr<ExpressionIR> right) {
    return std::make_unique<ExpressionIR>(
        std::in_place_type<BinOpIR>,
        op,
        std::move(left),
        std::move(right)
    );
}

std::string BinOpIR::label() {
    switch(op) {
        case ADD: return "ADD";
        case SUB: return "SUB";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case MOD: return "MOD";
        case AND: return "AND";
        case OR: return "OR";
        case XOR: return "XOR";
        case LSHIFT: return "LSHIFT";
        case RSHIFT: return "RSHIFT";
        case ARSHIFT: return "ARSHIFT";
        case EQ: return "EQ";
        case NEQ: return "NEQ";
        case LT: return "LT";
        case GT: return "GT";
        case LEQ: return "LEQ";
        case GEQ: return "GEQ";
    }
}
