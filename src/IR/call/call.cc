#include "call.h"
#include "IR/ir.h"
#include <memory>
#include <utility>

std::unique_ptr<ExpressionIR> CallIR::makeMalloc(std::vector<std::unique_ptr<ExpressionIR>> args) {
    return std::make_unique<ExpressionIR>(
        std::in_place_type<CallIR>,
        std::move(NameIR::makeMalloc()),
        std::move(args)
    );
}

std::unique_ptr<ExpressionIR> CallIR::makeException() {
    std::vector<std::unique_ptr<ExpressionIR>> args(0); // empty vector
    return std::make_unique<ExpressionIR>(
        std::in_place_type<CallIR>,
        std::move(NameIR::makeMalloc()),
        std::move(args)
    );
}
