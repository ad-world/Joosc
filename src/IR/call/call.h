#pragma once

#include "IR/ir.h"
#include <vector>
#include <string>

class CallIR {
protected:
    std::unique_ptr<ExpressionIR> target;
    std::vector<std::unique_ptr<ExpressionIR>> args;
public:
    CallIR(std::unique_ptr<ExpressionIR> target, std::vector<std::unique_ptr<ExpressionIR> > args) : target{std::move(target)}, args(args) {}
    ExpressionIR &target() { return *target; };
    std::vector<std::unique_ptr<ExpressionIR>> args() { return args; };
    int getNumArgs() { return args.size(); };
    std::string label() { return "CALL"; }
};