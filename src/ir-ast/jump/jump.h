#pragma once

#include <string>
#include <memory>
#include "IR/ir.h"

class JumpIR {
    std::unique_ptr<ExpressionIR> target;

  public:
    JumpIR(std::unique_ptr<ExpressionIR> target) : target{std::move(target)} {}

    ExpressionIR &target() { return *target; }

    std::string label() { return "JUMP"; }
};
