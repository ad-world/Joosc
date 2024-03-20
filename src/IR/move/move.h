#pragma once

#include <string>
#include <memory>
#include "IR/ir.h"

class MoveIR {
    std::unique_ptr<ExpressionIR> target; // The destination of the move
    std::unique_ptr<ExpressionIR> source; // The value to be moved

public:
    MoveIR(std::unique_ptr<ExpressionIR> target, std::unique_ptr<ExpressionIR> source) 
        : target{std::move(target)}, source{std::move(source)} {}

    ExpressionIR &getTarget() { return *target; }
    ExpressionIR &getSource() { return *source; }

    std::string label() { return "MOVE"; }
};
