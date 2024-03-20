#pragma once

#include <memory>
#include "IR/ir.h"
#include <string>
class CJumpIR {
    std::unique_ptr<ExpressionIR> cond;
    std::string true_label;
    std::string false_label;

public:
    CJumpIR(std::unique_ptr<ExpressionIR> cond, std::string true_label, std::string false_label) : cond{std::move(cond)}, true_label(true_label), false_label(false_label) {}
    CJumpIR(std::unique_ptr<ExpressionIR> cond, std::string true_label) : cond{std::move(cond)}, true_label(true_label), false_label("") {}
    ExpressionIR &condition() { return *cond; }
    std::string trueLabel() { return true_label; }
    std::string falseLabel() { return false_label; }
    std::string label() { return "CJUMP"; }
};
