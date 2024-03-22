#include "exp.h"
#include "IR/ir.h"

std::unique_ptr<StatementIR> ExpIR::makeStmt(std::unique_ptr<ExpressionIR> expr) {
    return std::make_unique<StatementIR>(
        std::in_place_type<ExpIR>,
        std::move(expr)
    );
}
