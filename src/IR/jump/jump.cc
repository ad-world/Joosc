#include "jump.h"
#include "IR/ir.h"

std::unique_ptr<StatementIR> JumpIR::makeStmt(std::unique_ptr<ExpressionIR> target) {
    return std::make_unique<StatementIR>(
        std::in_place_type<JumpIR>,
        std::move(target)
    );
}
