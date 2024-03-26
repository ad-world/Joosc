#include "mem.h"
#include "IR/ir.h"

std::unique_ptr<ExpressionIR> MemIR::makeExpr(std::unique_ptr<ExpressionIR> address) {
    return std::make_unique<ExpressionIR>(
        std::in_place_type<MemIR>,
        std::move(address)
    );
}
