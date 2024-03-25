#pragma once

#include "IR/ir_variant.h"

// Convert IR to Canonical IR form
class IRCanonicalizer {
  public:
    void operator()(IR&);
    void operator()(ExpressionIR&);
    void operator()(StatementIR&);

    void convert(IR&);
    void convert(ExpressionIR&);
    void convert(StatementIR&);
};
