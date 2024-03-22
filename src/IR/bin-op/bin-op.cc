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
