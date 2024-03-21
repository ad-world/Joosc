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

    ExpressionIR &getTarget() { assert(target.get()); return *target.get(); }
    ExpressionIR &getSource() { assert(source.get()); return *source.get(); }

    std::string label() { return "MOVE"; }
};
