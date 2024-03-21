#pragma once

#include "IR/ir_variant.h"
#include <memory>
#include <string>
#include <cassert>

class ReturnIR {
    std::unique_ptr<ExpressionIR> ret;

public:
    ReturnIR(std::unique_ptr<ExpressionIR> ret) : ret{std::move(ret)} {}

    ExpressionIR &getRet() { assert(ret.get()); return *ret.get(); }

    std::string label() { return "RETURN"; }
};
