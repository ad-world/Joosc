#pragma once

#include <memory>
#include "IR/ir_variant.h"
#include <string>
#include <cassert>

class ESeqIR {
    std::unique_ptr<StatementIR> stmt;
    std::unique_ptr<ExpressionIR> expr;

public:
    ESeqIR(std::unique_ptr<StatementIR> stmt, std::unique_ptr<ExpressionIR> expr)
        : stmt(std::move(stmt)), expr(std::move(expr)) {}
    StatementIR &getStmt() { assert(stmt.get()); return *stmt.get(); }
    ExpressionIR &getExpr() { assert(expr.get()); return *expr.get(); }
    std::string label() { return "ESEQ"; }
    bool isConstant() { return false; }
};
