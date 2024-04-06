#pragma once

#include "IR/ir.h"
#include <vector>
#include <memory>

// Return true iff IR is canonical
class CanonicalChecker {
    // For handling CJumps
    std::vector<std::unique_ptr<StatementIR>>* current_body;
    size_t current_statement;

    void check(StatementIR&);
    void check(ExpressionIR&);

    bool result = true;
    bool output_reason;

    void notCanonicalBecause(const std::string& reason);
  public:
    bool check(IR&, bool output_reason=false);
};
