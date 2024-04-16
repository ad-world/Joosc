#pragma once

#include "IR/ir_variant.h"
#include <memory>
#include <string>
#include <cassert>

class ReturnIR {
    std::unique_ptr<ExpressionIR> ret; // CANNOT BE NULL ANYMORE (causes a lot of errors - returns 0 for void function)

public:
    ReturnIR(std::unique_ptr<ExpressionIR> ret) : ret{std::move(ret)} {}

    ExpressionIR *getRet() { return ret.get(); }

    std::string label() { return "RETURN"; }

    static std::unique_ptr<StatementIR> makeStmt(std::unique_ptr<ExpressionIR> ret);
};
