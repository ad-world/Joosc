#include "temp.h"
#include "IR/ir.h"
#include <memory>
#include <utility>

size_t TempIR::num_temps = 0;
std::unique_ptr<ExpressionIR> TempIR::makeExpr(std::string str, bool isGlobal) {
    return std::make_unique<ExpressionIR>(std::in_place_type<TempIR>, str, isGlobal);
}
