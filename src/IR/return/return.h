#pragma once

#include "IR/ir.h"
#include <memory>
#include <string>

class ReturnIR {
    std::unique_ptr<ExpressionIR> ret;

public:
    ReturnIR(std::unique_ptr<ExpressionIR> ret) : ret{std::move(ret)} {}

    ExpressionIR &getRet() { return *ret.get(); }

    std::string label() { return "RETURN"; }
};
