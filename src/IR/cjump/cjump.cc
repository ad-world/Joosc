#include "cjump.h"
#include "IR/ir.h"

std::unique_ptr<StatementIR> CJumpIR::makeStmt(std::unique_ptr<ExpressionIR> cond, std::string true_label, std::string false_label) {
    return std::make_unique<StatementIR>(
        std::in_place_type<CJumpIR>,
        std::move(cond),
        true_label,
        false_label
    );
}
