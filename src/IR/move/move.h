#pragma once

#include <string>
#include <memory>
#include "IR/ir.h"

class MoveIR {
    std::unique_ptr<ExpressionIR> target; // The destination of the move
    std::unique_ptr<ExpressionIR> source; // The value to be moved

  public:
    MemIR(std::unique_ptr<ExpressionIR> target, std::unique_ptr<ExpressionIR> source) 
        : target{std::move(address)}, source{std::move(address)} {}

    ExpressionIR &target() { return *target; }
    ExpressionIR &source() { return *source; }

    std::string label() { return "MOVE"; }
};
