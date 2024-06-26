#include "expr-stack.h"
#include <iostream>

int ExprStack::popValue() {
    int value = stack.top().value;
    if (debugLevel > 1) {
        std::cout << "Popping value " << value << std::endl;
    }
    stack.pop();
    return value;
}

ExprStack::ExprStack(int debugLevel) : debugLevel(debugLevel) {}

StackItem ExprStack::pop() {
    StackItem item = stack.top();
    stack.pop();
    return item;
}

void ExprStack::pushAddr(int value, int addr) {
    if(debugLevel > 1) {
        std::cout << "Pushing MEM " << value << " (" << addr << ")" << std::endl;
    }
    stack.push(StackItem(value, addr));
}

void ExprStack::pushTemp(int value, std::string temp) {
    if(debugLevel > 1) {
        std::cout << "Pushing TEMP " << value << " (" << temp << ")" << std::endl;
    }
    stack.push(StackItem(StackItem::Kind::TEMP, value, temp));
}

void ExprStack::pushName(int value, std::string name) {
    if(debugLevel > 1) {
        std::cout << "Pushing NAME " << value << " (" << name << ")" << std::endl;
    }
    stack.push(StackItem(StackItem::Kind::NAME, value, name));
}

void ExprStack::pushValue(int value) {
    if(debugLevel > 1) {
        std::cout << "Pushing value " << value << std::endl;
    }
    stack.push(StackItem(value));
}