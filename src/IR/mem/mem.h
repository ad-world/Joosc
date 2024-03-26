#pragma once

#include <string>
#include <memory>
#include "IR/ir_variant.h"
#include <cassert>

class MemIR {
    std::unique_ptr<ExpressionIR> address;

  public:
    MemIR(std::unique_ptr<ExpressionIR> address) : address{std::move(address)} {}

    ExpressionIR &getAddress() { assert(address.get()); return *address.get(); }

    std::string label() { return "MEM"; }

    bool isConstant() { return false; }; 

    static std::unique_ptr<ExpressionIR> makeExpr(std::unique_ptr<ExpressionIR> address);
};
