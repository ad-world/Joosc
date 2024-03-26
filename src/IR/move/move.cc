#include "move.h"
#include "IR/ir.h"

std::unique_ptr<StatementIR> MoveIR::makeStmt(std::unique_ptr<ExpressionIR> target, std::unique_ptr<ExpressionIR> source) {
    return std::make_unique<StatementIR>(
        std::in_place_type<MoveIR>,
        std::move(target),
        std::move(source)
    );
}
