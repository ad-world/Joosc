#pragma once

#include <string>
#include <memory>
#include "IR/ir_variant.h"
#include <cassert>

class JumpIR {
    std::unique_ptr<ExpressionIR> target;

  public:
    JumpIR(std::unique_ptr<ExpressionIR> target) : target{std::move(target)} {}

    ExpressionIR &getTarget() { assert(target.get()); return *target.get(); }

    std::string label() { return "JUMP"; }

    static std::unique_ptr<StatementIR> makeStmt(std::unique_ptr<ExpressionIR> target);
};
