#pragma once

#include <stack>
#include <string>
#include "IR-interpreter/stack-item/stack-item.h"

class ExprStack {
    std::stack<StackItem> stack;
    int debugLevel = 0;
public:
    ExprStack() = default;
    ExprStack(int debugLevel);
    int popValue();
    StackItem pop();
    void pushAddr(int value, int addr);
    void pushTemp(int value, int addr);
    void pushName(int value, std::string name);
    void pushValue(int value);
};