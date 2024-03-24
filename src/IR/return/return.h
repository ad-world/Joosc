#pragma once

#include "IR/ir_variant.h"
#include <memory>
#include <string>
#include <cassert>

class ReturnIR {
    std::unique_ptr<ExpressionIR> ret; // CAN BE NULL

public:
    ReturnIR(std::unique_ptr<ExpressionIR> ret) : ret{std::move(ret)} {}

    ExpressionIR *getRet() { return ret.get(); }

    std::string label() { return "RETURN"; }

    static std::unique_ptr<StatementIR> makeStmt(std::unique_ptr<ExpressionIR> ret);
};
