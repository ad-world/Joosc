#include "IR/ir.h"
#include <string>
class CJumpIR {
    ExpressionIR *cond;
    std::string true_label;
    std::string false_label;

public:
    CJumpIR(ExpressionIR *cond, std::string true_label, std::string false_label) : cond(cond), true_label(true_label), false_label(false_label) {}
    CJumpIR(ExpressionIR *cond, std::string true_label) : cond(cond), true_label(true_label), false_label("") {}
    ExpressionIR *condition() { return cond; }
    std::string trueLabel() { return true_label; }
    std::string falseLabel() { return false_label; }
    std::string label() { return "CJUMP"; }
};