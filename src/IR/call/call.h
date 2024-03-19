#include "IR/ir.h"
#include <vector>
#include <string>

class CallIR {
protected:
    ExpressionIR *target;
    std::vector<ExpressionIR *> args;
public:
    CallIR(ExpressionIR *target, std::vector<ExpressionIR *> args) : target(target), args(args) {}
    ExpressionIR* target() { return target; };
    std::vector<ExpressionIR *> args() { return args; };
    int getNumArgs() { return args.size(); };
    std::string label() { return "CALL"; }
};