#pragma once

#include "IR/ir.h"
#include <string>

class ESeqIR {
    std::unique_ptr<StatementIR> stmt;
    std::unique_ptr<ExpressionIR> expr;

public:
    ESeqIR(std::unique_ptr<StatementIR> stmt, std::unique_ptr<ExpressionIR> expr) : stmt(std::move(stmt)), expr(std::move(expr)) {}
    StatementIR &stmt() { return *stmt; }
    ExpressionIR &expr() { return *expr; }
    std::string label() { return "ESEQ"; }
    bool isConstant() { return false; }
};