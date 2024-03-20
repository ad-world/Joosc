#pragma once

#include <string>
#include <memory>
#include "IR/ir.h"

class MemIR {
    std::unique_ptr<ExpressionIR> address;

  public:
    MemIR(std::unique_ptr<ExpressionIR> address) : address{std::move(address)} {}

    ExpressionIR &getAddress() { return *address; }

    std::string label() { return "MEM"; }

    bool isConstant() { return false; }; 
};
