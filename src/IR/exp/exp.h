#pragma once

#include "IR/ir_variant.h"
#include <cassert>
#include <memory>
#include <string>

class ExpIR {
    std::unique_ptr<ExpressionIR> expr;
public:
    ExpIR(std::unique_ptr<ExpressionIR> expr) : expr{std::move(expr)} {}
    ExpressionIR &getExpr() { assert(expr.get()); return *expr.get(); }
    std::string label() { return "EXP"; }
    static std::unique_ptr<StatementIR> makeStmt(std::unique_ptr<ExpressionIR> expr);
};
