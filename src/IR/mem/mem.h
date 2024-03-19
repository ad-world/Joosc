#pragma once

#include <string>
#include <memory>
#include "IR/ir.h"

class MemIR {
    std::unique_ptr<ExpressionIR> address;

  public:
    MemIR(std::unique_ptr<ExpressionIR> address) : address{std::move(address)} {}

    ExpressionIR &address() { return *address; }

    std::string label() { return "MEM"; }
};
