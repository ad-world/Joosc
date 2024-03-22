#include "IR/ir.h"
#include <utility>

std::unique_ptr<ExpressionIR> ConstIR::makeZero() {
    return std::make_unique<ExpressionIR>(std::in_place_type<ConstIR>, 0);
}

std::unique_ptr<ExpressionIR> ConstIR::makeOne() {
    return std::make_unique<ExpressionIR>(std::in_place_type<ConstIR>, 1);
}
