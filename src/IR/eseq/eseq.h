#pragma once

#include <memory>
#include "IR/ir.h"
#include <string>

class ESeqIR {
    std::unique_ptr<StatementIR> stmt;
    std::unique_ptr<ExpressionIR> expr;

public:
    ESeqIR(std::unique_ptr<StatementIR> stmt, std::unique_ptr<ExpressionIR> expr) : stmt(std::move(stmt)), expr(std::move(expr)) {}
    StatementIR &getStmt() { return *stmt.get(); }
    ExpressionIR &getExpr() { return *expr; }
    std::string label() { return "ESEQ"; }
    bool isConstant() { return false; }
};
