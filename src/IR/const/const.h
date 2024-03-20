#pragma once

#include "IR/ir.h"
#include <string>

class ConstIR {
    int value;
public:
    ConstIR(int value) : value(value) {}
    int value() { return value; }
    std::string label() { return "CONST"; }
    bool isConstant() { return true; }
};