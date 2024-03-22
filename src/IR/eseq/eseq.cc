#include "eseq.h"
#include "IR/ir.h"

std::unique_ptr<ExpressionIR> ESeqIR::makeExpr(std::unique_ptr<StatementIR> stmt, std::unique_ptr<ExpressionIR> expr) {
    return std::make_unique<ExpressionIR>(
        std::in_place_type<ESeqIR>,
        std::move(stmt),
        std::move(expr)
    );
}
