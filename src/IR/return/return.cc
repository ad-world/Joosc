#include "return.h"
#include "IR/ir.h"

std::unique_ptr<StatementIR> ReturnIR::makeStmt(std::unique_ptr<ExpressionIR> ret) {
    return std::make_unique<StatementIR>(
        std::in_place_type<ReturnIR>,
        std::move(ret)
    );
}
