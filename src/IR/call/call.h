#pragma once

#include <memory>
#include "IR/ir_variant.h"
#include <cassert>
#include <vector>
#include <string>

class CallIR {
protected:
    std::unique_ptr<ExpressionIR> target;
    std::vector<std::unique_ptr<ExpressionIR>> args;
public:
    CallIR(std::unique_ptr<ExpressionIR> target, std::vector<std::unique_ptr<ExpressionIR> > args)
        : target{std::move(target)}, args{std::move(args)} {}
    ExpressionIR &getTarget() { assert(target.get()); return *target.get(); };
    std::vector<std::unique_ptr<ExpressionIR>> &getArgs() { return args; };
    int getNumArgs() { return args.size(); };
    std::string label() { return "CALL"; }
    bool isConstant() { return false; };
};
