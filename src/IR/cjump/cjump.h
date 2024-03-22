#pragma once

#include <memory>
#include "IR/ir_variant.h"
#include <cassert>
#include <string>

class CJumpIR {
    std::unique_ptr<ExpressionIR> cond;
    std::string true_label;
    std::string false_label;

public:
    CJumpIR(std::unique_ptr<ExpressionIR> cond, std::string true_label, std::string false_label = "") : cond{std::move(cond)}, true_label(true_label), false_label(false_label) {}
    ExpressionIR &condition() { assert(cond.get()); return *cond.get(); }
    std::string trueLabel() { return true_label; }
    std::string falseLabel() { return false_label; }
    std::string label() { return "CJUMP"; }
    static std::unique_ptr<StatementIR> makeStmt(std::unique_ptr<ExpressionIR> cond, std::string true_label, std::string false_label = "");
};
