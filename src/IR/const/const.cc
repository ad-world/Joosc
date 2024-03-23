#include "IR/ir.h"
#include <utility>

std::unique_ptr<ExpressionIR> ConstIR::makeZero() {
    return makeExpr(0);
}

std::unique_ptr<ExpressionIR> ConstIR::makeOne() {
    return makeExpr(1);
}

std::unique_ptr<ExpressionIR> ConstIR::makeWords(int num_words) {
    return makeExpr(4 * num_words);
}

std::unique_ptr<ExpressionIR> ConstIR::makeExpr(int64_t value) {
    return std::make_unique<ExpressionIR>(std::in_place_type<ConstIR>, value);
}
