#pragma once

#include "IR/ir.h"
#include <memory>

class ReturnIR {
    std::unique_ptr<ExpressionIR> ret;

    public:
        ReturnIR(std::unique_ptr<ExpressionIR> ret) : ret{std::move(ret)} {}

        ExpressionIR &ret() { return ret; }

        std::string label() { return "RETURN"; }
};